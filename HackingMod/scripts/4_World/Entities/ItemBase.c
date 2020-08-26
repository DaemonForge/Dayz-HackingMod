modded class ItemBase extends InventoryItem
{
	protected int m_ECLE_HackID = 0;
	protected bool m_ECLE_HackInProgress = false;
	
	
	void ItemBase(){
		RegisterNetSyncVariableInt("m_ECLE_HackID");
		RegisterNetSyncVariableBool("m_ECLE_HackInProgress");
	}
	
	void ECLE_HackInit(int id){
		m_ECLE_HackID = id;
		m_ECLE_HackInProgress = true;
		SetSynchDirty();
	}
	
	int ECLE_GetHackID(){
		return m_ECLE_HackID;
	}
	
	void ECLE_InterruptHack(){
		m_ECLE_HackInProgress = false;
		SetSynchDirty();
	}
	
	void ECLE_CompleteHack(){
		m_ECLE_HackInProgress = false;
		m_ECLE_HackID = 0;
		SetSynchDirty();
	}
	
	/*
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
		{
			return false;
		}
		if (!GetHackingModConfig().NewSetup() && GetHackingModConfig() == "5"){
			if ( !ctx.Read( m_ECLE_HackID ) )
			{
				return false;
			}		
		}		
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_ECLE_HackID );
	}
	*/
};