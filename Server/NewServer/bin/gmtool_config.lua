

TL_CONFIG = {};

TL_CONFIG["TLConfig"] = {
	--nReconnectTime = 10000,
	nPingSvrTimeout = 30000000,
	nPingSvrInternal = 1000,
	nReconnectTime = 10000,

	nSmallPacketCount = 2048,
	nLargePacketCount = 32,

	nMaxAcceptor = 8,
	nMaxConnector = 32,
	nMaxSessionCount = 32,
	nSendBufSize = 1024 * 1024 * 1,
	nRecvBufSize = 1024 * 1024 * 1,
	nPacketBufferSize = 1024 * 1,


	nReLoginInternal = 10000,

	nCmdDataPoolCount = 100,

	szAddress = "127.0.0.1",
	wGMPort = 9011,

}


TL_CONFIG["GMList"] ={
	nSvrNum = 2,
	arrGMList = {
		[0] = {
			nSvrID = 0,
			szName = "",
			szAddr = "",
			dwPort = 9011,
		},
		[1] = {
			nSvrID = 1,
			szName = "Test One",
			szAddr = "127.0.0.1",
			dwPort = 9011,
		},
	},
}



function LoadTraverseTable(d, t)
    for k,v in pairs(t) do
		if d[k] == nil then
			print("variable "..k.." is nil")
		else
			if type(v) == "table" then
				LoadTraverseTable(d[k], v)
			else
				d[k] = t[k];
			end
		end
    end
end

function LoadFile()
	LoadTraverseTable(GetTLConfig(), TL_CONFIG);
end


