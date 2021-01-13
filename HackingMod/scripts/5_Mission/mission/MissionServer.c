modded class MissionServer extends MissionBase
{
	override void OnInit()
	{
		super.OnInit();
		Print("[HackingMod] OnInit");
		GetHackingModConfig();
		GetRPCManager().AddRPC( "HACK", "RPCHackingModSettings", this, SingeplayerExecutionType.Both );
	}
	
	
	void RPCHackingModSettings( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		PlayerIdentity RequestedBy = PlayerIdentity.Cast(sender);
		if (RequestedBy){
			GetRPCManager().SendRPC("HACK", "RPCHackingModSettings", new Param1< HackingModConfig >( GetHackingModConfig() ), true, RequestedBy);
		}
	}
	
}


