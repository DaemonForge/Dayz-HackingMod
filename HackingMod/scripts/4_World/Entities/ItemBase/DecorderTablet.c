class DecoderTablet extends ItemBase{
	
	protected bool m_HackingStarted = false;
	protected bool m_HackingInterrupted = false;
	protected bool m_HackingInterruptedLocal = false;
	
	protected bool m_HackingCompleted = false;
	protected bool m_HackingCompletedLocal = false;
	
	protected bool m_TabletON = false;
	protected bool m_TabletONLocal = false;
	
	protected float m_HackTimeRemaining;
	
	protected float HackingRadius = 10;
	
	protected bool RequireInHands = true;
		
	override void SetActions()
	{
		super.SetActions();

        AddAction(ActionHackCodeLock);
    }

	void DecoderTablet(){
		
	}
	
	void ~DecoderTablet(){
		if (m_HackingStarted && !m_HackingInterrupted && !m_HackingCompleted){ // to make less null pointers
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove(this.CheckHackProgress); 
		}
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
		SEffectManager.PlaySoundOnObject("landmine_loop_SoundSet", this);
		SetAnimationPhase("top",0);
	}
	
	void HackCompletedClient(){
		m_HackingCompletedLocal = m_HackingCompleted;
		SEffectManager.PlaySoundOnObject("combinationlock_open_SoundSet", this);
		SetAnimationPhase("top",0);
	}
	
	
	void StartHackServer(ItemBase HackingTarget, PlayerBase Hacker){
		PlayerBase hacker = PlayerBase.Cast(Hacker);
		ItemBase hackingTarget = ItemBase.Cast(HackingTarget);
		string heading = "HACK STARTED";
		string resumed = " has started";
		if (hacker && hackingTarget){
			m_HackingCompleted = false;
			m_HackingInterrupted = false;
			
			if ( hackingTarget.GetHackID() != this.GetHackID() || hackingTarget.GetHackID() == 0){ //Starting A hack on a new object
				m_HackingStarted = true;
				m_HackTimeRemaining = GetHackingModConfig().GetHackTime(hackingTarget.GetType()) * 1000;
				
				int HackID = GetGame().GetTime();
				HackID = HackID * 10000;
				HackID = HackID + Math.RandomInt(0,99999);
				this.HackInit(HackID);
				hackingTarget.HackInit(HackID);
				if (hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[HackingMod][Raid] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") has started hacking " + hackingTarget.GetType() + " with ID: " + HackID + " at " + hackingTarget.GetPosition());
				}
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") has started hacking " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition());
			} else {
				this.HackInit(GetHackID());
				hackingTarget.HackInit(GetHackID());
				heading = "HACK RESUMED";
				resumed = " has resumed";
				if (hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[HackingMod][Raid] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") has resumed hacking " + hackingTarget.GetType() + " with ID: " + GetHackID() + " at " + hackingTarget.GetPosition());
				}
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") has resumed hacking " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition());
			}
			SendPlayerMessage(hacker, heading, "The hacking of " + hackingTarget.GetDisplayName() + resumed);
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckHackProgress, 2000, false, hackingTarget, hacker);
			SetSynchDirty();
		}
	}
	
	void StartHackClient(){
		SetAnimationPhase("top",1);
		SEffectManager.PlaySoundOnObject("defibrillator_ready_SoundSet", this);
	}
	
	bool CheckHackInterrupt(ItemBase HackingTarget, PlayerBase Hacker){
		bool IsInterrupted = false;
		PlayerBase hacker = PlayerBase.Cast(Hacker);
		ItemBase hackingTarget = ItemBase.Cast(HackingTarget);
		if (hacker && hacker.GetIdentity() && hackingTarget){
            CodeLock codelock = CodeLock.Cast(hackingTarget.GetAttachmentByConfigTypeName("CodeLock"));
			HackableItem hackingData = GetHackingModConfig().GetHackableItem( hackingTarget.GetType() );
            if ( codelock && hackingData.Type != "") {
				if (!codelock.GetLockState()){
					//Code Lock not locked
					IsInterrupted = true;
					GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to codelock being unlocked");
				}
			} else { //Code Lock Removed
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to codelock missing");
			}
			//Missing Batteries
			if (!GetHackingModConfig().CanHack( hackingTarget.GetType(), this.CountBatteries())){
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to Missing Batteries");
			}
			//Random Chance Calcuations
			float DoInterrupt = Math.RandomFloat(0,1000);
			float InterruptChance = hackingData.ChanceOfInterrupt * 1000;
			if (DoInterrupt < InterruptChance){
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to interrupt Chance");
			}
			//Tablet is in hands
			if (hacker.GetItemInHands() != this && RequireInHands){
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to tablet dropped from hands");
			}
			//Hacker Left Radius
			if (vector.Distance(hackingTarget.GetPosition(), hacker.GetPosition()) > HackingRadius){
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to Hacker Leaving the hacking radius");
			}
			//Tablet is ruined
			if (this.IsRuined()){
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to Tablet Being Destroyed");
			}
			//Player has Disconnected
			if (hacker.IsPlayerDisconnected()){
				IsInterrupted = true;
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to Player Disconected");
			}
		} else {
			//This shouldn't happen, but Player and Target are inviald
			IsInterrupted = true;
			GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") had their hacking of " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition() + " Interrupted due to invalid Target or Player");
		}
		return IsInterrupted;
	}
	
	
	void CheckHackProgress(ItemBase HackingTarget, PlayerBase Hacker){
		ItemBase hackingTarget = ItemBase.Cast(HackingTarget);
		PlayerBase hacker = PlayerBase.Cast(Hacker);
		if (hackingTarget && hacker){
           	m_HackingInterrupted = CheckHackInterrupt(hackingTarget, hacker);
			if (!m_HackingInterrupted){
				m_HackTimeRemaining = m_HackTimeRemaining - 2000;
				if (m_HackTimeRemaining > 2000){
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckHackProgress, 2000, false, hackingTarget, hacker);
				} else if (m_HackTimeRemaining > 500){ //if its this close to finishing just finish
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.CheckHackProgress, m_HackTimeRemaining, false, hackingTarget, hacker);
				} else {
					m_HackTimeRemaining = 0; //incase it was made negitive
					GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(this.HackCompleted, 200, false, hackingTarget, hacker);
				}
			} else {
				hackingTarget.InterruptHack();
				this.InterruptHack();
				SendPlayerMessage(hacker, "HACK INTERRUPTED", "The hacking of " + hackingTarget.GetDisplayName() + " has been interrupted");
			}
		}else{
			m_HackingStarted = false;
			m_HackingInterrupted = false;
		}
		SetSynchDirty();
	}
	
	void HackCompleted(ItemBase HackingTarget, PlayerBase Hacker){
		PlayerBase hacker = PlayerBase.Cast(Hacker);
		ItemBase hackingTarget = ItemBase.Cast(HackingTarget);
		float itemMaxHealth = 0;
        if (hackingTarget && hacker) {
            CodeLock codelock = CodeLock.Cast(hackingTarget.GetAttachmentByConfigTypeName("CodeLock"));
			HackableItem hackingData = GetHackingModConfig().GetHackableItem( hackingTarget.GetType());
            if ( codelock && hackingData.Type != "") {
				itemMaxHealth = codelock.GetMaxHealth("", "Health");
				itemMaxHealth++;
				codelock.AddHealth("", "Health", -itemMaxHealth);
                codelock.UnlockServer(hacker, hackingTarget);
				hackingTarget.GetInventory().DropEntity(InventoryMode.SERVER, hackingTarget, codelock);
				#ifdef HEROESANDBANDITSMOD
					if (hacker.GetIdentity()){
						GetHeroesAndBandits().NewPlayerAction(hacker.GetIdentity().GetPlainId(), "Hack" + hackingData.Type + "Raid");
					}
				#endif
				if (hacker.GetIdentity() && GetHackingModConfig().ScriptLogging){
					Print("[HackingMod][Raid] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") Hacked  " + hackingTarget.GetType() + " with ID: " + GetHackID() + " at " + hackingTarget.GetPosition());
				}
				GetGame().AdminLog("[HackingMod][Raid][ID:" + GetHackID() + "] " + hacker.GetIdentity().GetName() + "(" +  hacker.GetIdentity().GetPlainId() + ") Hacked  " + hackingTarget.GetType() + " at " + hackingTarget.GetPosition());
					
				this.AddHealth("", "Health", -hackingData.TabletDamage);
				DestoryBatteries( hackingData.Batteries );
				
				this.CompleteHack();
				hackingTarget.CompleteHack();
				
				m_HackingStarted = false;
				m_HackingInterrupted = false;
				m_HackingCompleted = true;
            }
        }
		if (CountBatteries() == 0){
			m_TabletON = false;
		}
		if (m_HackingCompleted){
			SendPlayerMessage(Hacker, "HACK FINISHED", "The hacking of " + hackingTarget.GetDisplayName() + " has finished");
		}
		SetSynchDirty();
	}
	
	void TurnOnTablet(){
		m_TabletON = true;
		m_TabletONLocal = true;
		if(!IsRuined()){
			SetObjectTexture(0, "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("zbytek"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablet_on"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablef_off"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("top"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("background"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("screen"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("backscreen"), "HackingMod\\Data\\textures\\tablet_on_co.paa");
			SetObjectMaterial( GetHiddenSelectionIndex("backscreen"), "HackingMod\\Data\\textures\\Tablet_screen_on.rvmat" );
		}
	}
	
	void TurnOffTablet(){
		m_TabletON = false;
		m_TabletONLocal = false;
		if(!IsRuined()){
			SetObjectTexture(0, "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("zbytek"), "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablet_on"), "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("tablef_off"), "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("top"), "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("background"), "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectTexture( GetHiddenSelectionIndex("screen"), "HackingMod\\Data\\textures\\Tablet_off_co.paa");
			SetObjectMaterial( GetHiddenSelectionIndex("backscreen"), "HackingMod\\Data\\textures\\Tablet_on.rvmat" );
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx){
		super.OnStoreSave( ctx );
		
		ctx.Write( m_HackingStarted );
		ctx.Write( m_HackingInterrupted );
		ctx.Write( m_HackingCompleted );
		ctx.Write( m_HackTimeRemaining );
		ctx.Write( m_TabletON );
	}

	

	override bool OnStoreLoad( ParamsReadContext ctx, int version ){
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
		
		if ( loadingsuccessfull && m_HackTimeRemaining > 0 && !m_HackingCompleted ){
			m_HackingInterrupted = true;
		}
		
		SetSynchDirty();
		
		return loadingsuccessfull;
	}
	
	override void AfterStoreLoad(){	
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
		TabletBattery tempBattery1;
		TabletBattery tempBattery2;
		TabletBattery tempBattery3;
		TabletBattery tempBattery4;
		TabletBattery tempBattery5;
		
		if (Class.CastTo(tempBattery1, FindAttachmentBySlotName("Att_TabletBattery_1"))){
			if (!tempBattery1.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery2, FindAttachmentBySlotName("Att_TabletBattery_2"))){
			if (!tempBattery2.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery3, FindAttachmentBySlotName("Att_TabletBattery_3"))){
			if (!tempBattery3.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery4, FindAttachmentBySlotName("Att_TabletBattery_4"))){
			if (!tempBattery4.IsRuined()){
				count++;
			}
		}
		if (Class.CastTo(tempBattery5, FindAttachmentBySlotName("Att_TabletBattery_5"))){
			if (!tempBattery5.IsRuined()){
				count++;
			}
		}
		return count;
	}
	
	void DestoryBatteries( int number){
		int count = 0;
		TabletBattery tempBattery1;
		TabletBattery tempBattery2;
		TabletBattery tempBattery3;
		TabletBattery tempBattery4;
		TabletBattery tempBattery5;
		
		
		if (Class.CastTo(tempBattery5, FindAttachmentBySlotName("Att_TabletBattery_5"))){
			if (!tempBattery5.IsRuined()){
				tempBattery5.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery4, FindAttachmentBySlotName("Att_TabletBattery_4"))){
			if (!tempBattery4.IsRuined()){
				tempBattery4.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery3, FindAttachmentBySlotName("Att_TabletBattery_3"))){
			if (!tempBattery3.IsRuined()){
				tempBattery3.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery2, FindAttachmentBySlotName("Att_TabletBattery_2"))){
			if (!tempBattery2.IsRuined()){
				tempBattery2.AddHealth("", "Health", -21);
				count++;
			}
			if (count >= number){
				return;
			}
		}
		if (Class.CastTo(tempBattery1, FindAttachmentBySlotName("Att_TabletBattery_1"))){
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
		if (GetGame().IsServer() && (slot_name == "Att_TabletBattery_1" || slot_name == "Att_TabletBattery_2" || slot_name == "Att_TabletBattery_3" || slot_name == "Att_TabletBattery_4" || slot_name == "Att_TabletBattery_5")){
			m_TabletON = true;
			SetSynchDirty();
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name){
		super.EEItemDetached(item, slot_name);
		if (GetGame().IsServer() && (slot_name == "Att_TabletBattery_1" || slot_name == "Att_TabletBattery_2" || slot_name == "Att_TabletBattery_3"  || slot_name == "Att_TabletBattery_5"  || slot_name == "Att_TabletBattery_5")){
			if (CountBatteries() == 0){
				m_TabletON = false;
				SetSynchDirty();
			}
		}
	}
	
	void SendPlayerMessage(PlayerBase hacker, string heading, string text){
		string Heading = heading;
		string Message = text;
		PlayerBase Hacker = PlayerBase.Cast(hacker);
		string NotificationIcon = "HackingMod/GUI/Images/hacking.paa";
		if (Hacker.GetIdentity() && GetHackingModConfig().Notification == 0){
			HackingModNotifications.CreateNotification(new ref StringLocaliser(Heading), new ref StringLocaliser(Message), NotificationIcon, -938286307, 8, Hacker.GetIdentity());
		}
		if (Hacker.GetIdentity() && (GetHackingModConfig().Notification == 1 ||  GetHackingModConfig().Notification == 2)){
			GetRPCManager().SendRPC("HACK", "RPCHackingModNotifcation", new Param3< string, string, float >( Heading, Message, 8 ), true, Hacker.GetIdentity());
		}
	}
	
};