modded class ItemBase extends InventoryItem
{
	protected int m_HackID = 0;
	protected bool m_HackInProgress = false;
	
	
	void ItemBase(){
		RegisterNetSyncVariableInt("m_HackID");
		RegisterNetSyncVariableBool("m_HackInProgress");
	}
	
	void HackInit(int id){
		m_HackID = id;
		m_HackInProgress = true;
		SetSynchDirty();
	}
	
	int GetHackID(){
		return m_HackID;
	}
	
	void InterruptHack(){
		m_HackInProgress = false;
		SetSynchDirty();
	}
	
	void CompleteHack(){
		m_HackInProgress = false;
		m_HackID = 0;
		SetSynchDirty();
	}
	
	/*
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
		{
			return false;
		}
		if (!GetHackingModConfig().NewSetup()){
			if ( !ctx.Read( m_HackID ) )
			{
				return false;
			}		
		}		
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_HackID );
	}
	*/
};