"""Verify GPU query records without requiring graphics hardware."""


def harness(game, source, function):
    engine = game.parent
    parts = ["namespace GpuInfoTests {", "enum { MAX_GPU_INFO_LEN = 1024 };"]
    parts.append(function(source(engine / "Engine/Inc/RHI.h"), "struct FGPUInfo\n") + ";")
    for driver in ["D3D9", "D3D11", "OpenGL"]:
        name = "F" + driver + "DynamicRHI"
        parts.append("struct " + name + " { FGPUInfo GpuInfo; void QueryGPUHardWareInfo(FGPUInfo& info); };")
        parts.append(function(source(engine / (driver + "Drv/Src/" + driver + "Commands.cpp")),
                              "void " + name + "::QueryGPUHardWareInfo("))
    parts.append(r'''
template<class Driver> void CheckCopy() {
    Driver driver;
    FGPUInfo output;
    memset(output.DeviceName, 0xa5, sizeof(output.DeviceName));
    driver.QueryGPUHardWareInfo(output);
    for (char byte : output.DeviceName) assert(byte == 0);
    memcpy(driver.GpuInfo.DeviceName, "Adapter", 8);
    driver.QueryGPUHardWareInfo(output);
    assert(!strcmp(output.DeviceName, "Adapter"));
}
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(FGPUInfo) unsigned char bytes[sizeof(FGPUInfo)];
        memset(bytes, pattern, sizeof(bytes));
        FGPUInfo* info = new(bytes) FGPUInfo;
        for (char byte : info->DeviceName) assert(byte == 0);
        info->~FGPUInfo();
        FOpenGLDynamicRHI driver;
        FGPUInfo output;
        memset(output.DeviceName, pattern, sizeof(output.DeviceName));
        driver.QueryGPUHardWareInfo(output);
        for (char byte : output.DeviceName) assert(byte == 0);
        assert(strlen(output.DeviceName) == 0);
    }
    CheckCopy<FD3D9DynamicRHI>(); CheckCopy<FD3D11DynamicRHI>();
    puts("GPU info initialization and unavailable-query output regressions passed");
}
}
''')
    return "\n".join(parts)
