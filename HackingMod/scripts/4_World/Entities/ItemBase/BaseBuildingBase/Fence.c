modded class Fence extends BaseBuildingBase
{
	
	protected bool m_Locked = false;
	protected string m_Code = "";
	protected bool m_HasCode = false;
	
	void Fence()
	{
		RegisterNetSyncVariableBool( "m_Locked" );
		RegisterNetSyncVariableBool( "m_HasCode" );
		
		SetSynchDirty();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction( ExpansionActionEnterCodeLock );
		AddAction( ExpansionActionChangeCodeLock );
	}

	override bool IsOpened()
	{
		return m_IsOpened;
	}
	
	override bool ExpansionIsOpenable()
	{
		return !m_IsOpened && !IsLocked();
	}
	
	override void Lock()
	{
		if ( m_HasCode )
		{
			m_Locked = true;
		}

		SetSynchDirty();
	}
	
	override void Unlock()
	{
		m_Locked = false;

		SetSynchDirty();
	}	
	
	override void FailedUnlock()
	{
		
	}
	
	override void SetCode( string code )
	{
		if (code == "")
		{
			m_HasCode = false;
		} 
		else
		{
			m_HasCode = true;
		}

		m_Code = code;
		m_Locked = false;

		SetSynchDirty();
	}
	
	override string GetCode()
	{
		return m_Code;
	}

	
	override bool HasCode()
	{
		return m_HasCode;
	}
	
	override bool ExpansionHasCodeLock( string selection )
	{
		ExpansionCodeLock codelock = ExpansionCodeLock.Cast(FindAttachmentBySlotName( "Att_CombinationLock" )) ;
		if ( codelock ) //check if attachment is code lock
		{
			return true;
		}
		return super.ExpansionHasCodeLock( selection );
	}

	override ExpansionCodeLock ExpansionGetCodeLock()
	{
		return ExpansionCodeLock.Cast(FindAttachmentBySlotName( "Att_CombinationLock" ));
	}

	override bool IsLocked()
	{
		if ( m_Locked ) 
		{
			return true;
		}
		return super.IsLocked();
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave( ctx );
				
		ctx.Write( m_Locked );
		ctx.Write( m_Code );
		ctx.Write( m_HasCode );
	}


	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
		{
			return false;
		}
		
		if ( !ctx.Read( m_Locked ) )
		{
			return false;
		}
		
		if ( !ctx.Read( m_Code ) )
		{
			return false;
		}

		if ( !ctx.Read( m_HasCode ) )
		{
			return false;
		}	
		
		return true;
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		if ( item && ( slot_name == "Att_CombinationLock" ) && HasCode() )
		{
			Unlock();
			SetCode("");
		}
	}
	
	override void OnPartDismantledServer(notnull Man player, string part_name, int action_id) {
        ConstructionPart constrution_part = GetConstruction().GetConstructionPart(part_name);

        if (constrution_part.IsGate()) {
            if (IsLocked()) {
                Unlock();
            }
            SetCode("");
        }
        super.OnPartDismantledServer(player, part_name, action_id);
    }
	
	override void OnPartDestroyedServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart(part_name);

        if (constrution_part.IsGate()) {
            if (IsLocked()) {
                Unlock();
            }
            SetCode("");
			if (ExpansionHasCodeLock("")){
			 	ExpansionCodeLock codelock = ExpansionCodeLock.Cast(FindAttachmentBySlotName( "Att_CombinationLock" ));
				if (codelock){
					float health = codelock.GetHealth("", "");
					codelock.AddHealth("", "Health", -health);
				}
			}
        }
		super.OnPartDestroyedServer( player, part_name, action_id );
	}
};
