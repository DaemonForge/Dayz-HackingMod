modded class MissionGameplay extends MissionBase
{
	
	void MissionGameplay(){
		GetRPCManager().AddRPC( "HACK", "RPCHackingModSettings", this, SingeplayerExecutionType.Both );
	}
	
	void RPCHackingModSettings( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param1< HackingModConfig > data  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		m_HackingModConfig = data.param1;
	}
}