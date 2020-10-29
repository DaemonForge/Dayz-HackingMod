class CfgPatches
{
	class HackingModDefines
	{
		requiredVersion=0.1;
		requiredAddons[]={
		};
	};
};

class CfgMods
{
	class HackingModDefines
	{
		dir="HackingModDefines";
        name="HackingMod";
        credits="DaemonForge";
        author="DaemonForge";
        authorID="0";
        version="0.1";
        extra=0;
        type="mod";
	    dependencies[]={ "Game", "World", "Mission"};
	    class defs
	    {
			class gameScriptModule
            {
				value = "";
                files[]={
					"HackingModDefines/scripts/Common"
					};
            };
			
			class worldScriptModule
            {
                value="";
                files[]={ 
					"HackingModDefines/scripts/Common"
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]={
					"HackingModDefines/scripts/Common"
				};
            };
        };
    };
};