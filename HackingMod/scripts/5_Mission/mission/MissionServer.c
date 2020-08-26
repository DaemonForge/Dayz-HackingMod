modded class MissionServer extends MissionBase
{
	override void OnInit()
	{
		super.OnInit();
		GetHackingModConfig();
		if(GetHackingModConfig().NewSetup()){
			Print("[ExpansionCodeLock] Calling m_ExpansionCodeLockConfig SetupComplete");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(m_ExpansionCodeLockConfig, "SetupComplete", 1200 * 1000, false); //Wait over 10 minutes just to be safe worst case is server stops before this and it doesn't finish the setup and hacking in progress won't be able to be resumed
		}
	}
	
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if ( identity )
		{
			string playerID = identity.GetPlainId();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "SendExpansionCodeLocksExpandedSettings", 1500, false, new Param1<ref PlayerBase >( player ));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "SendExpansionCodeLocksExpandedSettings", 3500, false, new Param1<ref PlayerBase >( player ));
		}
	}
			
	void SendExpansionCodeLocksExpandedSettings( PlayerBase player ){
		if (player.IsPlayerDisconnected()) { return; }
		PlayerIdentity identity = player.GetIdentity();
		if (identity){
			GetRPCManager().SendRPC("ECLE", "RPCExpansionCodeLocksExpandedSettings", new Param1< ExpansionCodeLockConfig >( GetHackingModConfig() ), true, identity);
		}
	}
	
}


