modded class MissionGameplay extends MissionBase
{

	protected ref array<ref HackingModNotificationWidget> HackingNotifications = new ref array<ref HackingModNotificationWidget>;
	
	void MissionGameplay(){
		GetRPCManager().AddRPC( "HACK", "RPCHackingModSettings", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "HACK", "RPCHackingModNotifcation", this, SingeplayerExecutionType.Both );
	}
	
	void RPCHackingModSettings( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param1< HackingModConfig > data  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		m_HackingModConfig = data.param1;
		Print("[HackingMod][Client] Settings Received");
		m_HackingModConfig.HackableItems.Debug();
	}
	
	void RPCHackingModNotifcation( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param3< string, string, float > data  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		string Heading = data.param1;
		string Message = data.param2;
		float MessageTime = data.param3;
		if (GetHackingModConfig().Notification == 1) {
			GetGame().Chat( Message, "colorAction");
		} else if (GetHackingModConfig().Notification == 2) {
			int LastIndex = HackingNotifications.Count() - 1;
			if (HackingNotifications.Count() > 1) {
				HackingNotifications.Get(LastIndex).EarlyProccess();
			}
			int nextIndex = HackingNotifications.Count();
			HackingNotifications.Insert(new ref HackingModNotificationWidget());
			HackingNotifications.Get(nextIndex).Init(Heading, Message, "HackingMod/GUI/Images/hacking.paa", MessageTime);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.DeleteNotification, MessageTime * 1500, false);
		}
	}
		
	void DeleteNotification(){
		if ( HackingNotifications.Count() > 0 ){
			HackingNotifications.RemoveOrdered(0);
		}
	}
}