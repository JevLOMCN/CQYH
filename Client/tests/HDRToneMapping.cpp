#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdio>

template<class T> struct Com {
    T* p = nullptr;
    ~Com() { if (p) p->Release(); }
    T* operator->() { return p; }
};
static void Require(HRESULT value, const char* operation) {
    if (FAILED(value)) { char text[256]; sprintf_s(text, "%s failed: 0x%08lX", operation, value); throw std::runtime_error(text); }
}
static std::string Read(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Cannot read shader source");
    std::ostringstream text; text << input.rdbuf(); return text.str();
}
static std::string Function(const std::string& source, const std::string& signature) {
    const auto start = source.find(signature);
    if (start == std::string::npos) throw std::runtime_error("Missing production shader function");
    auto end = source.find('{', start); int depth = 1;
    if (end == std::string::npos) throw std::runtime_error("Missing shader function body");
    while (++end < source.size() && depth) { if (source[end] == '{') ++depth; if (source[end] == '}') --depth; }
    if (depth) throw std::runtime_error("Unbalanced shader function");
    return source.substr(start, end - start);
}
struct Shader { Com<IDirect3DPixelShader9> code; Com<ID3DXConstantTable> constants; };
struct Gpu {
    Com<IDirect3D9> d3d;
    Com<IDirect3DDevice9> device;
    Com<IDirect3DSurface9> target, readback;
    Gpu() {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(nullptr); wc.lpszClassName = L"UE3HDRShaderTest";
        RegisterClassW(&wc);
        // A device needs an HWND. This test window is never shown or presented.
        HWND window = CreateWindowW(wc.lpszClassName, L"UE3 HDR shader test", WS_OVERLAPPEDWINDOW, 0,0,32,32,nullptr,nullptr,wc.hInstance,nullptr);
        if (!window) throw std::runtime_error("Cannot create hidden device window");
        d3d.p = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d.p) throw std::runtime_error("Direct3D9 unavailable");
        D3DPRESENT_PARAMETERS pp = {}; pp.Windowed = TRUE; pp.SwapEffect = D3DSWAPEFFECT_DISCARD; pp.hDeviceWindow = window;
        pp.BackBufferWidth = pp.BackBufferHeight = pp.BackBufferCount = 1; pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        Require(d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,window,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_FPU_PRESERVE,&pp,&device.p),"CreateDevice");
        Require(device->CreateRenderTarget(1,1,D3DFMT_A32B32G32R32F,D3DMULTISAMPLE_NONE,0,FALSE,&target.p,nullptr),"Create float render target");
        Require(device->CreateOffscreenPlainSurface(1,1,D3DFMT_A32B32G32R32F,D3DPOOL_SYSTEMMEM,&readback.p,nullptr),"Create float readback");
        Require(device->SetRenderTarget(0,target.p),"Set render target");
        device->SetDepthStencilSurface(nullptr); device->SetRenderState(D3DRS_ZENABLE,FALSE); device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE); device->SetRenderState(D3DRS_SRGBWRITEENABLE,FALSE);
    }
    void Compile(Shader& shader, const std::string& source, const char* entry) {
        Com<ID3DXBuffer> code, errors;
        HRESULT result = D3DXCompileShader(source.data(), static_cast<UINT>(source.size()),nullptr,nullptr,entry,"ps_3_0",D3DXSHADER_OPTIMIZATION_LEVEL3,&code.p,&errors.p,&shader.constants.p);
        if (FAILED(result) && errors.p) std::fprintf(stderr,"%s\n",static_cast<const char*>(errors->GetBufferPointer()));
        Require(result,"Compile production HLSL");
        Require(device->CreatePixelShader(static_cast<const DWORD*>(code->GetBufferPointer()),&shader.code.p),"Create pixel shader");
    }
    void Constant(Shader& shader, const char* name, const float* value) {
        auto handle = shader.constants->GetConstantByName(nullptr,name);
        if (!handle) throw std::runtime_error("Missing shader constant");
        Require(shader.constants->SetFloatArray(device.p,handle,value,4),"Set shader constant");
    }
    void Draw(Shader& shader, float* result) {
        struct Vertex { float x,y,z,w,u,v; };
        const Vertex quad[] = {{-.5f,-.5f,0,1,0,0},{.5f,-.5f,0,1,1,0},{-.5f,.5f,0,1,0,1},{.5f,.5f,0,1,1,1}};
        Require(device->SetPixelShader(shader.code.p),"Set pixel shader");
        Require(device->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1),"Set vertex format");
        Require(device->BeginScene(),"BeginScene");
        Require(device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,quad,sizeof(Vertex)),"Draw test pixel");
        Require(device->EndScene(),"EndScene");
        Require(device->GetRenderTargetData(target.p,readback.p),"Read GPU result");
        D3DLOCKED_RECT data; Require(readback->LockRect(&data,nullptr,D3DLOCK_READONLY),"Lock result");
        memcpy(result,data.pBits,4*sizeof(float)); readback->UnlockRect();
    }
};
static int Checks = 0, Failures = 0;
static void Check(bool passed, const char* name) {
    ++Checks; if (!passed) { ++Failures; std::printf("FAIL: %s\n",name); }
}
static bool Near(float a, float b) { return std::isfinite(a) && std::fabs(a-b) <= 1e-4f * (1+std::fabs(b)); }
int wmain(int argc, wchar_t** argv) {
    try {
        if (argc != 2) throw std::runtime_error("Expected shader directory argument");
        std::filesystem::path path(argv[1]); Gpu gpu; Shader exposure, tone;
        gpu.Compile(exposure,"sampler2D CurrentLum; float g_fElapsedTime; float4 HDRLuminanceParameter;\n"+Function(Read(path/L"LuminanceAndToneMapping.usf"),"float4 CalculateAdaptedLumPS"),"CalculateAdaptedLumPS");
        gpu.Compile(tone,"float4 TestColor; float4 TestLuminance;\n"+Function(Read(path/L"UberPostProcessBlendPixelShader.usf"),"float3 ToneMapping(")+"\nfloat4 TestPS(float2 uv:TEXCOORD0):COLOR { return float4(ToneMapping(TestColor.rgb,TestLuminance),1); }","TestPS");
        Check(exposure.constants->GetConstantByName(nullptr,"CurrentLum") == nullptr,"HDR has no unbound texture dependency");
        Check(exposure.constants->GetConstantByName(nullptr,"g_fElapsedTime") == nullptr,"HDR has no unbound frame-time dependency");
        const float controls[][4] = {{0,.5f,1,.6f},{.0015f,.505f,.91f,.49f},{0,.5f,1,.5f}};
        for (const auto& input : controls) {
            float actual[4]; gpu.Constant(exposure,"HDRLuminanceParameter",input); gpu.Draw(exposure,actual);
            Check(Near(actual[0],input[3]) && Near(actual[1],input[2]) && Near(actual[2],input[0]) && Near(actual[3],input[1]),"authored HDR controls reach GPU unchanged in legacy channel order");
        }
        const float luminance[] = {.6f,1,0,.5f};
        for (int i=0;i<=16;++i) {
            const float gray = i*.25f, color[] = {gray,gray,gray,1}; float actual[4];
            gpu.Constant(tone,"TestColor",color); gpu.Constant(tone,"TestLuminance",luminance); gpu.Draw(tone,actual);
            // With white=1 Reinhard simplifies to color * exposure, including HDR values above 1.
            Check(Near(actual[0],gray*1.2f) && Near(actual[1],gray*1.2f) && Near(actual[2],gray*1.2f),"neutral-white ramp preserves the authored exposure");
        }
        const float cases[][8] = {
            {0,0,0,1, .6f,1,0,.5f},
            {.25f,.25f,.25f,1, .6f,1,.5f,.5f},
            {.5f,.5f,.5f,1, .6f,1,.5f,.5f},
            {.25f,.25f,.25f,1, .6f,1,0,0},
            {.25f,.25f,.25f,1, .6f,0,0,.5f},
            {-.1f,.2f,.3f,1, .6f,1,0,.5f}
        };
        for (int n=0;n<6;++n) {
            float actual[4]; gpu.Constant(tone,"TestColor",cases[n]); gpu.Constant(tone,"TestLuminance",cases[n]+4); gpu.Draw(tone,actual);
            Check(std::isfinite(actual[0]) && std::isfinite(actual[1]) && std::isfinite(actual[2]) && actual[0]>=0 && actual[1]>=0 && actual[2]>=0,"black points, zero divisors and negative channels stay finite and nonnegative");
            if (n<3) {
                const bool black = Near(actual[0],0) && Near(actual[1],0) && Near(actual[2],0);
                if (!black) std::printf("Black-point case %d returned (%g,%g,%g)\n",n,actual[0],actual[1],actual[2]);
                Check(black,"black and clipped black point map to black");
            }
        }
        std::printf("HDR GPU shader checks: %d passed, %d failed\n",Checks-Failures,Failures);
        return Failures ? 1 : 0;
    } catch (const std::exception& e) { std::fprintf(stderr,"HDR shader test error: %s\n",e.what()); return 2; }
}
