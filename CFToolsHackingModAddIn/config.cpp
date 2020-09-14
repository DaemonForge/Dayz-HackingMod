class CfgPatches
{
	class CFToolsHackingModAddIn
	{
		requiredVersion=0.1;
		requiredAddons[]={
			
		};
	};
};

class CfgMods
{
	class CFToolsHackingModAddIn
	{
		dir="CFToolsHackingModAddIn";
        name="CFToolsHackingModAddIn";
        credits="DaemonForge";
        author="DaemonForge";
        authorID="0";
        version="0.1";
        extra=0;
        type="mod";
	    dependencies[]={ "Game", "World"};
	    class defs
	    {
			class gameScriptModule
            {
				value = "";
                files[]={
					"CFToolsHackingModAddIn/scripts/3_Game"
					};
            };
			
			class worldScriptModule
            {
                value="";
                files[]={ 
					"CFToolsHackingModAddIn/scripts/4_World" 
				};
            };
			
        };
    };
};
