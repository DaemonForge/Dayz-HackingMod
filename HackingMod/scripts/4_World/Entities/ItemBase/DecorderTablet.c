class DecoderTablet extends ItemBase{
	
	protected bool m_HackingStarted = false;
	protected bool m_HackingInterrupted = false;
	protected bool m_HackingInterruptedLocal = false;
	
	protected bool m_HackingCompleted = false;
	protected bool m_HackingCompletedLocal = false;
	
	protected bool m_TabletON = false;
	protected bool m_TabletONLocal = false;
	
	protected float m_HackTimeRemaining;
		
	override void SetActions()
	{
		super.SetActions();

        AddAction(ActionHackCodeLockOnTent);
        AddAction(ActionHackCodeLockOnDoor);
    }

	void DecoderTablet(){
		
	}
	
	void ~DecoderTablet()
	{
		
	}

	bool HasHackingStarted(){
		return m_HackingStarted;
	}

	bool WasHackingInterrupted(){
		return m_HackingInterrupted;
	}
		
	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableBool("m_HackingStarted");
		RegisterNetSyncVariableBool("m_HackingInterrupted");
		RegisterNetSyncVariableFloat("m_HackTimeRemaining");
		RegisterNetSyncVariableBool("m_HackingCompleted");
		RegisterNetSyncVariableBool("m_TabletON");
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( m_HackingInterrupted && !m_HackingInterruptedLocal )
		{
			HackInterruptedClient();
		} else if ( !m_HackingInterrupted){
			m_HackingInterruptedLocal = m_HackingInterrupted;
		}
		
		if ( m_HackingCompleted && !m_HackingCompletedLocal )
		{
			HackCompletedClient();
		} else if ( !m_HackingCompleted){
			m_HackingCompletedLocal = m_HackingCompleted;
		}
		
		if ( m_TabletON && !m_TabletONLocal ) {
			TurnOnTablet();
		} else if ( !m_TabletON && m_TabletONLocal ) {
			TurnOffTablet();
		}
		
	}
	
	void HackInterruptedClient(){
		m_HackingInterruptedLocal = m_HackingInterrupted;
		SEffectManager.PlaySoundOnObject("landmine_end_SoundSet", this);
		SetAnimationPhase("top",0);
	}
	
	void HackCompletedClient(){
		m_HackingCompletedLocal = m_HackingCompleted;
		SEffectManager.PlaySoundOnObject("Expansion_CodeLock_Unlock_SoundSet", this);
		SetAnimationPhase("top",0);
	}
	
	
	void StartHackServer(ItemBase hackingTarget, PlayerBase hacker){
		PlayerBase Hacker = PlayerBase.Cast(hacker);
		ItemBase HackingTarget = ItemBase.Cast(hackingTarget);
		if (Hacker && HackingTarget){
			m_HackingCompleted = false;
			m_HackingInterrupted = false;
			
			if ( HackingTarget.ECLE_GetHackID() != this.ECLE_GetHackID() || HackingTarget.ECLE_GetHackID() == 0){ //Starting A hack on a new object
				m_HackingStarted = true;
				m_HackTimeRemaining = GetHackingModConfig().HackingTimeTents * 1000;
				
				if( BaseBuildingBase.Cast(hackingTarget)){
					m_HackTimeRemaining = GetHackingModConfig().HackingTimeDoors * 1000;
				}
				
				int HackID = GetGame().GetTime();
				HackID = HackID * 10000;
				HackID = HackID + Math.RandomInt(0,10000);
				this.ECLE_HackInit(HackID);
				HackingTarget.ECLE_HackInit(HackID);
				if (Hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[CodeLockExpanded][Raid] " + Hacker.GetIdentity().GetName() + "(" +  Hacker.GetIdentity().GetPlainId() + ") has started hacking " + HackingTarget.GetType() + " with ID: " + HackID + " at " + HackingTarget.GetPosition());
				}
			} else {
				this.ECLE_HackInit(ECLE_GetHackID());
				HackingTarget.ECLE_HackInit(ECLE_GetHackID());
				if (Hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[CodeLockExpanded][Raid] " + Hacker.GetIdentity().GetName() + "(" +  Hacker.GetIdentity().GetPlainId() + ") has resumed hacking " + HackingTarget.GetType() + " with ID: " + ECLE_GetHackID() + " at " + HackingTarget.GetPosition());
				}
			}
			SendPlayerMessage(Hacker, "Hack Started", "The hacking of " + hackingTarget.GetDisplayName() + " has started");
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckHackProgress, 2000, false, hackingTarget, hacker);
			SetSynchDirty();
		}
	}
	
	void StartHackClient(){
		SetAnimationPhase("top",1);
		SEffectManager.PlaySoundOnObject("defibrillator_ready_SoundSet", this);
	}
	
	void CheckHackProgress(ItemBase hackingTarget, PlayerBase hacker){
		PlayerBase Hacker = PlayerBase.Cast(hacker);
		ItemBase HackingTarget = ItemBase.Cast(hackingTarget);
		if (Hacker && HackingTarget){
			
			TentBase tent = TentBase.Cast(hackingTarget); 
			BaseBuildingBase door = BaseBuildingBase.Cast(hackingTarget);
			
			if (door && CountBatteries() < GetHackingModConfig().BatteriesDoors){
				m_HackingInterrupted = true;
			}
			if (tent && CountBatteries() < GetHackingModConfig().BatteriesTents){
				m_HackingInterrupted = true;
			}
			
			if (!HackingTarget.IsLocked()){
				m_HackingInterrupted = true;
			}
			
			float DoInterrupt = Math.RandomFloat(0,1);
			float InterruptChance = GetHackingModConfig().ChanceOfInterrupt;
			if (DoInterrupt < InterruptChance){
				m_HackingInterrupted = true;
			}
			
			
			if (!m_HackingInterrupted && !HackingTarget.IsRuined() && vector.Distance(HackingTarget.GetPosition(), Hacker.GetPosition()) < 10 && Hacker.GetItemInHands() == this){
				m_HackTimeRemaining = m_HackTimeRemaining - 2000;
				if (m_HackTimeRemaining > 2000){
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckHackProgress, 2000, false, hackingTarget, Hacker);
				} else if (m_HackTimeRemaining > 500){ //if its this close to finishing just finish
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckHackProgress, m_HackTimeRemaining, false, hackingTarget, Hacker);
				} else {
					m_HackTimeRemaining = 0; //incase it was made negitive
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.HackCompleted, 200, false, hackingTarget, Hacker);
				}
			} else {
				HackingTarget.ECLE_InterruptHack();
				this.ECLE_InterruptHack();
				m_HackingInterrupted = true;
				SendPlayerMessage(Hacker, "Hack Interrupted", "The hacking of " + hackingTarget.GetDisplayName() + " has been interrupted");
			}
		}else{
			m_HackingStarted = false;
			m_HackingInterrupted = false;
		}
		SetSynchDirty();
	}
	
	void HackCompleted(ItemBase hackingTarget, PlayerBase hacker){
		PlayerBase Hacker = PlayerBase.Cast(hacker);
		BaseBuildingBase HackingTarget = BaseBuildingBase.Cast(hackingTarget);
		float itemMaxHealth = 0;
		float toolDamage = GetHackingModConfig().HackSawDamage;
        TentBase tent = TentBase.Cast(hackingTarget); 
        if (tent && Hacker) {
            ExpansionCodeLock codelock = ExpansionCodeLock.Cast(tent.FindAttachmentBySlotName( "Att_ExpansionCodeLock" ));
			
            if (codelock ) {
				itemMaxHealth = codelock.GetMaxHealth("", "Health");
				itemMaxHealth++;
				codelock.AddHealth("", "Health", -itemMaxHealth);
				toolDamage++;
                tent.Unlock();
                tent.SetCode( "" );
				tent.GetInventory().DropEntity(InventoryMode.SERVER, tent, codelock);
				#ifdef HEROESANDBANDITSMOD
					if (Hacker.GetIdentity()){
						GetHeroesAndBandits().NewPlayerAction(Hacker.GetIdentity().GetPlainId(), "HackExpansionCodeLockDoorRaid");
					}
				#endif
				if (Hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[CodeLockExpanded][Raid] " + Hacker.GetIdentity().GetName() + "(" +  Hacker.GetIdentity().GetPlainId() + ") Hacked  " + tent.GetType() + " with ID: " + ECLE_GetHackID() + " at " + tent.GetPosition());
				}
				this.AddHealth("", "Health", -GetHackingModConfig().TabletDamageTents);
				DestoryBatteries( GetHackingModConfig().BatteriesTents );
				
				tent.ECLE_CompleteHack();
				this.ECLE_CompleteHack();
				
				m_HackingStarted = false;
				m_HackingInterrupted = false;
				m_HackingCompleted = true;
            }
        }
		if (HackingTarget && Hacker) {
            ExpansionCodeLock codelock2 = ExpansionCodeLock.Cast(HackingTarget.GetAttachmentByConfigTypeName("ExpansionCodeLock"));
			
            if (codelock2 ) {
				itemMaxHealth = codelock2.GetMaxHealth("", "Health");
				itemMaxHealth++;
				codelock2.AddHealth("", "Health", -itemMaxHealth);
				toolDamage++;
                HackingTarget.Unlock();
                HackingTarget.SetCode( "" );
				HackingTarget.GetInventory().DropEntity(InventoryMode.SERVER, HackingTarget, codelock2);
				#ifdef HEROESANDBANDITSMOD
					if (Hacker.GetIdentity()){
						GetHeroesAndBandits().NewPlayerAction(Hacker.GetIdentity().GetPlainId(), "HackExpansionCodeLockTentRaid");
					}
				#endif
				if (Hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[CodeLockExpanded][Raid] " + Hacker.GetIdentity().GetName() + "(" +  Hacker.GetIdentity().GetPlainId() + ") Hacked  " + HackingTarget.GetType() + " with ID: " + ECLE_GetHackID() + " at " + HackingTarget.GetPosition());
				}
				this.AddHealth("", "Health", -GetHackingModConfig().TabletDamageDoors);
				DestoryBatteries( GetHackingModConfig().BatteriesDoors );
				
				HackingTarget.ECLE_CompleteHack();
				this.ECLE_CompleteHack();
				
				m_HackingStarted = false;
				m_HackingInterrupted = false;
				m_HackingCompleted = true;
            }
        }
		if (CountBatteries() == 0){
			m_TabletON = false;
		}
		if (m_HackingCompleted){
			SendPlayerMessage(Hacker, "Hack Finished", "The hacking of " + hackingTarget.GetDisplayName() + " has finished");
		}
		SetSynchDirty();
	}
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);	
		AddHealth( "GlobalHealth", "Health", -damageResult.GetDamage( "", "Health" ) );
	}

	void TurnOnTablet(){
		m_TabletON = true;
		m_TabletONLocal = true;
		if(!IsRuined()){
			SetObjectTexture(0, "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("zbytek"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablet_on"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablef_off"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("top"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("background"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("screen"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("backscreen"), "ExpansionCLExpanded\\Data\\textures\\ECLE_tablet_on_co.paa");
			SetObjectMaterial( GetHiddenSelectionIndex("backscreen"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_screen_on.rvmat" );
		}
	}
	
	void TurnOffTablet(){
		m_TabletON = false;
		m_TabletONLocal = false;
		if(!IsRuined()){
			SetObjectTexture(0, "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("zbytek"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablet_on"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablef_off"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("top"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("background"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectTexture(GetHiddenSelectionIndex("screen"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_off_co.paa");
			SetObjectMaterial( GetHiddenSelectionIndex("backscreen"), "ExpansionCLExpanded\\Data\\textures\\ECLE_Tablet_on.rvmat" );
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_HackingStarted );
		ctx.Write( m_HackingInterrupted );
		ctx.Write( m_HackingCompleted );
		ctx.Write( m_HackTimeRemaining );
		ctx.Write( m_TabletON );
	}

	

	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
		{
			return false;
		}
		bool loadingsuccessfull = true;
		if ( !ctx.Read( m_HackingStarted ) )
		{
			loadingsuccessfull = false;
		}
		if ( !ctx.Read( m_HackingInterrupted ) )
		{
			loadingsuccessfull = false;
		}
		if ( !ctx.Read( m_HackingCompleted ) )
		{
			loadingsuccessfull = false;
		}
		if ( !ctx.Read( m_HackTimeRemaining ) )
		{
			loadingsuccessfull = false;
		}
		if ( !ctx.Read( m_TabletON ) )
		{
			loadingsuccessfull = false;
		}
		
		if ( loadingsuccessfull && m_HackingStarted && m_HackTimeRemaining > 0 && !m_HackingCompleted ){
			m_HackingInterrupted = true;
		}
		
		SetSynchDirty();
		
		return loadingsuccessfull;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
		
		if (m_TabletON){
			 TurnOnTablet();
		} else {
			TurnOffTablet();
		}
		SetSynchDirty();
	}
	
	int CountBatteries(){
		int count = 0;
		ECLETabletBattery tempBattery1;
		ECLETabletBattery tempBattery2;
		ECLETabletBattery tempBattery3;
		ECLETabletBattery tempBattery4;
		
		if (Class.CastTo(tempBattery1, FindAttachmentBySlotName("Att_ECLETabletBattery_1"))){
			if (!tempBattery1.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery2, FindAttachmentBySlotName("Att_ECLETabletBattery_2"))){
			if (!tempBattery2.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery3, FindAttachmentBySlotName("Att_ECLETabletBattery_3"))){
			if (!tempBattery3.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery4, FindAttachmentBySlotName("Att_ECLETabletBattery_4"))){
			if (!tempBattery4.IsRuined()){
				count++;
			}
		}
		return count;
	}
	
	void DestoryBatteries( int number){
		int count = 0;
		ECLETabletBattery tempBattery1;
		ECLETabletBattery tempBattery2;
		ECLETabletBattery tempBattery3;
		ECLETabletBattery tempBattery4;
		
		if (Class.CastTo(tempBattery4, FindAttachmentBySlotName("Att_ECLETabletBattery_4"))){
			if (!tempBattery4.IsRuined()){
				tempBattery4.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery3, FindAttachmentBySlotName("Att_ECLETabletBattery_3"))){
			if (!tempBattery3.IsRuined()){
				tempBattery3.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery2, FindAttachmentBySlotName("Att_ECLETabletBattery_2"))){
			if (!tempBattery2.IsRuined()){
				tempBattery2.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery1, FindAttachmentBySlotName("Att_ECLETabletBattery_1"))){
			if (!tempBattery1.IsRuined()){
				tempBattery1.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
	}
	
	override void EEItemAttached(EntityAI item, string slot_name){
		super.EEItemAttached(item, slot_name);
		if (GetGame().IsServer() && (slot_name == "Att_ECLETabletBattery_1" || slot_name == "Att_ECLETabletBattery_2" || slot_name == "Att_ECLETabletBattery_3" || slot_name == "Att_ECLETabletBattery_4")){
			m_TabletON = true;
			SetSynchDirty();
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (GetGame().IsServer() && (slot_name == "Att_ECLETabletBattery_1" || slot_name == "Att_ECLETabletBattery_2" || slot_name == "Att_ECLETabletBattery_3"  || slot_name == "Att_ECLETabletBattery_4" )){
			if (CountBatteries() == 0){
				m_TabletON = false;
				SetSynchDirty();
			}
		}
		
	}
	
	void SendPlayerMessage(PlayerBase hacker, string heading, string text){
		PlayerBase Hacker = PlayerBase.Cast(hacker);
		if (Hacker.GetIdentity()){
			string Heading = heading;
			string Message = text;
			string Icon = "ExpansionCLExpanded/GUI/Images/hacking.paa";
			GetNotificationSystem().CreateNotification(new ref StringLocaliser(Heading), new ref StringLocaliser(Message), Icon, -938286307, 15, Hacker.GetIdentity());
		}
	}
	
};