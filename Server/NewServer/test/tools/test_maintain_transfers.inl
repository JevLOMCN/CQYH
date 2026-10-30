// Included by the real offscreen Qt model fixture; all packets stay in memory.
void transfer_tests(const fs::path& root, Allocator& allocator, Session& socket, QTreeWidgetItem& item)
{
    auto& sender=CTransmitFileMgr::GetInstance();
    const UINT32 ip=0x0100007f;
    const auto path=(root/"transfer.bin").string();
    const char* paths[]{path.c_str()};
    const char* names[]{"transfer.bin"};
    const auto write=[&](std::size_t size) {
        std::string contents(size,'\0');
        for (std::size_t i=0;i<size;++i) contents[i]=static_cast<char>((i*31+17)%251);
        std::ofstream stream{path,std::ios::binary|std::ios::trunc};stream.write(contents.data(),contents.size());
        if (!stream) throw std::runtime_error("cannot write transfer fixture");
        return contents;
    };
    const auto acknowledgement=[](const N2U_SEND_FILES& chunk) {
        U2N_SEND_FILES_ACK ack{};ack.wMsg=u2n_send_files_ack;ack.nErrorCode=errSuccess;
        std::memcpy(ack.szFileName,chunk.szFileName,sizeof(ack.szFileName));
        ack.wFileIndex=chunk.wFileIndex;ack.wFileTotal=chunk.wFileTotal;
        ack.wBufferIndex=chunk.wBufferIndex;ack.wBufferTotal=chunk.wBufferTotal;
        return ack;
    };
    const auto start=[&] { return sender.StartSending(1,ip,1,names,paths,etftPatch); };
    for (const std::size_t size: {0u,1u,32767u,32768u,32769u,65536u,327681u})
    {
        socket.chunks.clear();const auto contents=write(size);
        check(start(),"valid transfer starts with owned file streams");
        check(!start() && sender.IsSending(1,ip),"overlapping start preserves the existing transfer");
        for (int loop=0;loop<4;++loop) check(sender.MainLoop(),"bounded transfer poll keeps running");
        const auto blocks=std::max<std::size_t>(1,(size+32767)/32768);
        check(socket.chunks.size()==blocks,"chunk count covers empty, partial and exact block multiples without an extra packet");
        std::string received;
        for (std::size_t i=0;i<socket.chunks.size();++i)
        {
            const auto& chunk=*socket.chunks[i];
            check(chunk.wBufferIndex==i && chunk.wBufferTotal==blocks && chunk.wFileIndex==0 && chunk.wFileTotal==1 &&
                chunk.nFileType==etftPatch && std::string_view{chunk.szFileName}==names[0],"chunk indices, type and name preserve the protocol");
            const auto expected=std::min<std::size_t>(32768,size-received.size());
            check(chunk.wBufferSize==expected,"wire payload has the exact remaining byte count");
            received.append(chunk.szBuffer,chunk.wBufferSize);
            check(std::ranges::all_of(std::span{chunk.szBuffer}.subspan(chunk.wBufferSize),[](char c){return c==0;}),
                "unused fixed packet bytes are initialized");
        }
        check(received==contents,"binary transfer payload matches its source byte for byte");
        check(sender.IsSending(1,ip) && item.text(4)=="100%","sending waits for final acknowledgement before reporting success");
        if (!socket.chunks.empty())
        {
            auto ack=acknowledgement(*socket.chunks.back());
            ++ack.wFileTotal;
            check(!sender.Acknowledge(1,ip,ack) && sender.IsSending(1,ip),"mismatched acknowledgement cannot finish a transfer");
            --ack.wFileTotal;std::strcpy(ack.szFileName,"other.bin");
            check(!sender.Acknowledge(1,ip,ack) && sender.IsSending(1,ip),"unrelated filename acknowledgement cannot finish a transfer");
            ack=acknowledgement(*socket.chunks.back());deliver(allocator,socket,ack);
            check(!sender.IsSending(1,ip) && item.text(4)=="send all ok","actual ACK handler detaches completed transfer before updating GUI");
            deliver(allocator,socket,ack);
            check(!sender.IsSending(1,ip),"queued duplicate ACK after completion is harmless");
        }
    }
    write(1);
    const char* invalidNames[]{"../escape"};
    check(!sender.StartSending(1,ip,1,invalidNames,paths,etftPatch),"relative destination names are rejected");
    const std::string longName(128,'x');invalidNames[0]=longName.c_str();
    check(!sender.StartSending(1,ip,1,invalidNames,paths,etftPatch),"wire filenames cannot silently truncate");
    invalidNames[0]="CON.txt";
    check(!sender.StartSending(1,ip,1,invalidNames,paths,etftPatch),"reserved destination names are rejected");
    check(!sender.StartSending(1,ip,0,names,paths,etftPatch) &&
        !sender.StartSending(1,ip,9,names,paths,etftPatch) &&
        !sender.StartSending(1,ip,1,names,paths,etftInvalid),"file counts and transfer type are checked before reading arrays");
    const char* duplicateNames[]{"same.bin","SAME.bin"};
    const char* duplicatePaths[]{path.c_str(),path.c_str()};
    check(!sender.StartSending(1,ip,2,duplicateNames,duplicatePaths,etftPatch) && !sender.IsSending(1,ip),
        "duplicate destinations fail transactionally");
    const auto missing=(root/"absent.bin").string();const char* missingPaths[]{path.c_str(),missing.c_str()};
    const char* twoNames[]{"first.bin","second.bin"};
    check(!sender.StartSending(1,ip,2,twoNames,missingPaths,etftPatch) && !sender.IsSending(1,ip),
        "a later missing source releases earlier opened files without registering a transfer");
    fs::rename(path,path+".released");fs::rename(path+".released",path);
    check(!sender.StartSending(2,ip,1,names,paths,etftPatch),"unconfigured peers cannot acquire transfer resources");
    std::array<std::string,8> nameStorage;
    std::array<const char*,8> eightNames{},eightPaths{};
    for (std::size_t i=0;i<8;++i) { nameStorage[i]="file"+std::to_string(i)+".bin";eightNames[i]=nameStorage[i].c_str();eightPaths[i]=path.c_str(); }
    check(sender.StartSending(1,ip,8,eightNames.data(),eightPaths.data(),etftVersion),"all eight supported file slots are usable");
    socket.chunks.clear();
    for (int file=0;file<8;++file)
    {
        sender.MainLoop();const auto count=socket.chunks.size();sender.MainLoop();
        check(count==file+1 && socket.chunks.size()==count,"next file waits for the previous file acknowledgement");
        deliver(allocator,socket,acknowledgement(*socket.chunks.back()));
    }
    check(!sender.IsSending(1,ip),"eight-file transfer completes in order");
    write(65536);
    socket.chunks.clear();check(start(),"cancellation fixture starts");
    socket.onChunk=[&](const auto&) { sender.StopSending(1,ip);sender.MainLoop(); };
    sender.MainLoop();socket.onChunk={};
    check(socket.chunks.size()==1 && !sender.IsSending(1,ip),"reentrant cancellation closes streams and prevents the next chunk");
    fs::rename(path,path+".closed");fs::rename(path+".closed",path);
    check(start(),"reload fixture starts");
    socket.chunks.clear();socket.onChunk=[&](const auto&) { sender.Reload(); };
    sender.MainLoop();socket.onChunk={};
    check(socket.chunks.size()==1 && !sender.IsSending(1,ip),"reentrant reload releases active transfer snapshots");
    write(1);socket.chunks.clear();check(start(),"replacement fixture starts");
    socket.onChunk=[&](const auto&) {
        sender.StopSending(1,ip);check(start(),"callback can create a replacement transfer");sender.MainLoop();
    };
    sender.MainLoop();socket.onChunk={};
    check(socket.chunks.size()==1 && sender.IsSending(1,ip),"old poll neither sends nor cancels a replacement transfer");
    sender.MainLoop();deliver(allocator,socket,acknowledgement(*socket.chunks.back()));
    check(!sender.IsSending(1,ip),"replacement completes independently");
    socket.chunks.clear();check(start(),"synchronous acknowledgement fixture starts");
    socket.onChunk=[&](const auto& chunk) { deliver(allocator,socket,acknowledgement(chunk)); };
    sender.MainLoop();socket.onChunk={};
    check(!sender.IsSending(1,ip) && item.text(4)=="send all ok","synchronous final ACK cannot be overwritten by stale poll progress");
    for (const bool throws: {false,true})
    {
        check(start(),"failed send fixture starts");
        socket.rejectSend=!throws;socket.throwSend=throws;sender.MainLoop();socket.rejectSend=false;socket.throwSend=false;
        check(!sender.IsSending(1,ip) && allocator.packets.empty() && item.text(4)=="send files error",
            "rejected or throwing send releases packet and transfer resources");
    }
    write(65536);check(start(),"truncation fixture starts");fs::resize_file(path,1);socket.chunks.clear();sender.MainLoop();
    check(!sender.IsSending(1,ip) && socket.chunks.empty(),"short read aborts without sending uninitialized bytes");
    write(1);check(start(),"growth fixture starts");
    { std::ofstream stream{path,std::ios::binary|std::ios::app};stream.put('x'); }
    socket.chunks.clear();sender.MainLoop();
    check(!sender.IsSending(1,ip) && socket.chunks.empty(),"source growth aborts before publishing the final chunk");
    write(1);socket.chunks.clear();check(start(),"remote error fixture starts");sender.MainLoop();
    auto ack=acknowledgement(*socket.chunks.back());ack.nErrorCode=-2;deliver(allocator,socket,ack);
    check(!sender.IsSending(1,ip) && item.text(4)=="overwrite version config error","remote error stops resources and preserves the legacy error display");
    write(327681);socket.chunks.clear();check(start(),"early ACK fixture starts");sender.MainLoop();
    check(socket.chunks.size()==10,"poll respects the ten-chunk per-peer budget");
    ack=acknowledgement(*socket.chunks.back());
    check(!sender.Acknowledge(1,ip,ack) && sender.IsSending(1,ip),"successful ACK for an intermediate chunk cannot finish the file");
    ack.wBufferIndex=10;
    check(!sender.Acknowledge(1,ip,ack),"ACK for an unsent chunk is rejected");
    sender.StopSending(1,ip);
    write(1);
    auto now=std::chrono::steady_clock::time_point{};
    CTransmitFileMgr timed{[&] { return now; }};
    check(timed.StartSending(1,ip,1,names,paths,etftPatch),"clock-controlled timeout fixture starts");
    timed.MainLoop();now+=std::chrono::seconds{59};timed.MainLoop();
    check(timed.IsSending(1,ip),"acknowledgement remains pending before its timeout");
    now+=std::chrono::seconds{1};timed.MainLoop();
    check(!timed.IsSending(1,ip) && item.text(4)=="send files error","missing acknowledgement expires and releases transfer resources");
    check(start() && sender.Reload() && !sender.IsSending(1,ip),"reload closes pending streams even before any chunks are sent");
    fs::rename(path,path+".reload-closed");fs::rename(path+".reload-closed",path);
    check(sender.UnInit() && allocator.packets.empty(),"transfer fixture leaves no native or packet resources");
}
