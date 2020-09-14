class CFAPIItemHacked : RestCallback {
    ref CFServerPlugin cftools = new CFServerPlugin;
    
    override void OnError( int errorCode ){
        string log = "CFAPIItemHacked => ErrorCode("+errorCode+")";
        GetGame().AdminLog(log);
    };

    override void OnTimeout() {
        GetGame().AdminLog("CFAPIItemHacked => TimeoutError");
    };

    override void OnSuccess( string data, int dataSize ) {
        string log = "CFAPIItemHacked => " + data +" (" + dataSize + ")";
        if(cftools.debug_level) GetGame().AdminLog(log);
    };
};