modded class TentBase extends ItemBase
{
	
	protected bool m_Locked= false;
	protected string m_Code = "";
	protected bool m_HasCode = false;
	protected bool m_IsOpened = true;
	protected bool m_IsOpened1 = true;
	protected bool m_IsOpened2 = true;
	protected bool m_IsOpened3 = true;
	protected bool m_IsOpened4 = true;
	protected bool m_IsOpened5 = true;
	protected bool m_IsOpened6 = true;
	
	void TentBase()
	{
		RegisterNetSyncVariableBool( "m_Locked" );
		RegisterNetSyncVariableBool( "m_HasCode" );
		RegisterNetSyncVariableBool( "m_IsOpened" );
		RegisterNetSyncVariableBool( "m_IsOpened1" );
		RegisterNetSyncVariableBool( "m_IsOpened2" );
		RegisterNetSyncVariableBool( "m_IsOpened3" );
		RegisterNetSyncVariableBool( "m_IsOpened4" );
		RegisterNetSyncVariableBool( "m_IsOpened5" );
		RegisterNetSyncVariableBool( "m_IsOpened6" );
		
		SetSynchDirty();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction( ExpansionActionEnterCodeLock );
		AddAction( ExpansionActionChangeCodeLock );
	}
	
	void ToggleTentOpening(string state){
		if (state == "entrancec"){ //toggleing an closed door so it would now be open
			m_IsOpened = true;
		} else if (state == "entranceo"){  //toggleing an open door so it would now be closed
			m_IsOpened = false;
		} else { //Might Be Party Tent
			//Party Tent Logic
			if (state == "door1o"){      m_IsOpened1 = false; }
			else if (state == "door1c"){ m_IsOpened1 = true;  }
			else if (state == "door2o"){ m_IsOpened2 = false; }
			else if (state == "door2c"){ m_IsOpened2 = true;  }
			else if (state == "door3o"){ m_IsOpened3 = false; }
			else if (state == "door3c"){ m_IsOpened3 = true;  }
			else if (state == "door4o"){ m_IsOpened4 = false; }
			else if (state == "door4c"){ m_IsOpened4 = true;  }
			else if (state == "door5o"){ m_IsOpened5 = false; }
			else if (state == "door5c"){ m_IsOpened5 = true;  }
			else if (state == "door6o"){ m_IsOpened6 = false; }
			else if (state == "door6c"){ m_IsOpened6 = true;  }
			else { return; } //Wasn't Party Tent :) Probly a large tent no need to continue any more :)
			
			if ( m_IsOpened1 || m_IsOpened2 || m_IsOpened3 || m_IsOpened4 || m_IsOpened5 || m_IsOpened6 ){ //if any doors are open its now a closed door
				m_IsOpened = true;
			} else {
				m_IsOpened = false;
			}
		}
		SetSynchDirty();
	}
	
	override bool CanReceiveItemIntoCargo(EntityAI cargo) {
        if (m_Locked && GetHackingModConfig()){
			if (GetHackingModConfig().AllowCodeLocksOnTents){
           	 	return false;
			}
		}
        return super.CanReleaseCargo(cargo);
    }


    override bool CanReleaseCargo(EntityAI cargo) {
        if (m_Locked && GetHackingModConfig()){
			if (GetHackingModConfig().AllowCodeLocksOnTents){
           	 	return false;
			}
		}
        return super.CanReleaseCargo(cargo);
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId) {
        if (m_Locked && GetHackingModConfig()){
			if (GetHackingModConfig().AllowCodeLocksOnTents){
           	 	return false;
			}
		}
        return super.CanReceiveAttachment(attachment, slotId);
    }
	
	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if (m_Locked && GetHackingModConfig()){
			if (GetHackingModConfig().AllowCodeLocksOnTents){
           	 	return false;
			}
		}
        return super.CanReleaseAttachment(attachment);
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
			if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
			{
				//SEffectManager.PlaySound("Expansion_CodeLock_Locks_SoundSet", GetPosition());
			}
		}

		SetSynchDirty();
	}
	
	override void Unlock()
	{
		m_Locked = false;
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			//SEffectManager.PlaySound("Expansion_CodeLock_Unlock_SoundSet", GetPosition());
		}
		SetSynchDirty();
	}	
	
	override void FailedUnlock()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			//SEffectManager.PlaySound("Expansion_Shocks_SoundSet", GetPosition());
		}
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
		if ( ExpansionCodeLock.Cast(FindAttachmentBySlotName( "Att_ExpansionCodeLock" )) )
		{
			return true;
		}
		return false;
	}

	override ExpansionCodeLock ExpansionGetCodeLock()
	{
		return ExpansionCodeLock.Cast(FindAttachmentBySlotName( "Att_ExpansionCodeLock" ));
	}

	override bool IsLocked()
	{
		return m_HasCode && m_Locked;
	}
	
	override bool CanBePacked()
	{
		if (IsLocked()){
			return false;
		}
		return super.CanBePacked();
	}


	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_Locked );
		ctx.Write( m_Code );
		ctx.Write( m_HasCode );
		ctx.Write( m_IsOpened );
		ctx.Write( m_IsOpened1 );
		ctx.Write( m_IsOpened2 );
		ctx.Write( m_IsOpened3 );
		ctx.Write( m_IsOpened4 );
		ctx.Write( m_IsOpened5 );
		ctx.Write( m_IsOpened6 );
	}


	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
		{
			return false;
		}
		bool loadingsuccessfull = true;
			if ( !ctx.Read( m_Locked ) )
			{
				m_Locked = false;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_Code ) )
			{
				m_Code = "";
				loadingsuccessfull = false;
			}

			if ( !ctx.Read( m_HasCode ) )
			{
				m_HasCode = false;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_IsOpened ) )
			{
				m_IsOpened = true;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_IsOpened1 ) )
			{
				m_IsOpened1 = true;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_IsOpened2 ) )
			{
				m_IsOpened2 = true;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_IsOpened3 ) )
			{
				m_IsOpened3 = true;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_IsOpened4 ) )
			{
				m_IsOpened4 = true;
				loadingsuccessfull = false;
			}
				
			if ( !ctx.Read( m_IsOpened5 ) )
			{
				m_IsOpened5 = true;
				loadingsuccessfull = false;
			}
			
			if ( !ctx.Read( m_IsOpened6 ) )
			{
				m_IsOpened6 = true;
				loadingsuccessfull = false;
			}
		
		if ((loadingsuccessfull && !GetHackingModConfig().AllowCodeLocksOnTents)){ //If Code Locks on the tents it will remove them Just calling later so simplify and ensure that the code lock has been created
				GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( this.ExpansionCodeLockRemove, 1000, false );
		}
		
		SetSynchDirty();
		
		return loadingsuccessfull;
	}
	
	void ExpansionCodeLockRemove(){
		if (!GetHackingModConfig().AllowCodeLocksOnTents || GetHackingModConfig().NewSetup()){
			if (m_Locked || m_HasCode || ExpansionHasCodeLock("codelock") ){
				m_Locked = false;
				m_Code = "";
				m_HasCode = false;
				Print("[ExpansionCodeLock] Reseting " + GetType() + " at " + GetPosition());
				ExpansionCodeLock codelock = ExpansionCodeLock.Cast(FindAttachmentBySlotName( "Att_ExpansionCodeLock" ));
				if (codelock){
					Print("[ExpansionCodeLock] Deleteing Code Lock off Attachment Slot in " + GetType() + " at " + GetPosition());
					codelock.Delete();
				}
				ExpansionCodeLock codelock2 = ExpansionCodeLock.Cast(GetAttachmentByConfigTypeName("ExpansionCodeLock"));
				if (codelock2){
					Print("[ExpansionCodeLock] Deleteing Code Lock off Attachment by type " + GetType() + " at " + GetPosition());
					codelock2.Delete();
				}
				SetSynchDirty();
			}
		}
	}
	
	override bool CanDisplayAttachmentCategory(string category_name) {
		
        if (category_name == "Attachments" && GetHackingModConfig()) {
            return GetHackingModConfig().AllowCodeLocksOnTents;
        }

        return super.CanDisplayAttachmentCategory(category_name);
    }
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		if ( item && ( slot_name == "Att_ExpansionCodeLock" ) && HasCode() )
		{
			SetCode("");
		}
	}
};
