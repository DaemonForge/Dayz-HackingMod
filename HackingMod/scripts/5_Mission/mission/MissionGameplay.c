modded class MissionGameplay extends MissionBase
{
	override void OnInit()
	{
		super.OnInit();
		GetRPCManager().AddRPC( "ECLE", "RPCExpansionCodeLocksExpandedSettings", this, SingeplayerExecutionType.Both );
	}
	
	void RPCExpansionCodeLocksExpandedSettings( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param1< ExpansionCodeLockConfig > data  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		m_ExpansionCodeLockConfig = data.param1;
	}
}