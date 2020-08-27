modded class MissionServer extends MissionBase
{
	override void OnInit()
	{
		super.OnInit();
		GetHackingModConfig();
	}
	
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if ( identity )
		{
			string playerID = identity.GetPlainId();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "SendHackingModSettings", 1800, false, new Param1<ref PlayerBase >( player ));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "SendHackingModSettings", 4000, false, new Param1<ref PlayerBase >( player ));
		}
	}
			
	void SendHackingModSettings( PlayerBase player ){
		if (player.IsPlayerDisconnected()) { return; }
		PlayerIdentity identity = player.GetIdentity();
		if (identity){
			GetRPCManager().SendRPC("HACK", "RPCHackingModSettings", new Param1< HackingModConfig >( GetHackingModConfig() ), true, identity);
		}
	}
	
}


