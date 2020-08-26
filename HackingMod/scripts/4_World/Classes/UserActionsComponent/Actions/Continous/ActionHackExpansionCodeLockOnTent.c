class ActionHackExpansionCodeLockOnTentCB extends ActionContinuousBaseCB {
    override void CreateActionComponent() {
		float circleTime = GetHackingModConfig().HackStartTimeTents; 
      	m_ActionData.m_ActionComponent = new CAContinuousRepeat(circleTime);
    }
};

class ActionHackExpansionCodeLockOnTent : ActionContinuousBase {

	bool continueHack = false;
    void ActionHackExpansionCodeLockOnTent() {
        m_CallbackClass = ActionHackExpansionCodeLockOnTentCB;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
    }

    override void CreateConditionComponents() {
        m_ConditionItem = new CCINonRuined();
        m_ConditionTarget = new CCTNone();
    }

    override string GetText() {
		if (continueHack){
			return "Resume Hacking";
		}
        return "Hack Codelock";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) {
	    if (GetHackingModConfig().AllowHackingDoors){
			TentBase tent = TentBase.Cast(target.GetParent());
			ECLETablet tablet = ECLETablet.Cast(item);
			
			if (tent && tablet) {
				//Hacking is fresh Start
				if (tent.IsLocked() && !tablet.IsRuined()){
					ExpansionCodeLock codelock = ExpansionCodeLock.Cast(tent.GetAttachmentByConfigTypeName("ExpansionCodeLock"));
					if ( tablet.WasHackingInterrupted() && tablet.ECLE_GetHackID() == tent.ECLE_GetHackID() ) {
						continueHack = true;
						if ( tablet.CountBatteries() >= GetHackingModConfig().BatteriesTents && codelock) {
							return true;
						}
					} else if(!tablet.HasHackingStarted() || tablet.WasHackingInterrupted()) {
						continueHack = false;
						if ( tablet.CountBatteries() >= GetHackingModConfig().BatteriesTents && codelock ) {
							return true;
						}
					}
				}
	        }
		}
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data) {

        ItemBase target = ItemBase.Cast(action_data.m_Target.GetParent());    
		ECLETablet tablet = ECLETablet.Cast(action_data.m_MainItem);
		PlayerBase sourcePlayer = PlayerBase.Cast(action_data.m_Player);

        if (tablet && sourcePlayer && target) {
            tablet.StartHackServer(target, sourcePlayer);
        }
    }
	
	protected void OnFinishProgressClient( ActionData action_data )
	{  
		ECLETablet tablet = ECLETablet.Cast(action_data.m_MainItem);

        if (tablet) {
            tablet.StartHackClient();
        }
	}
};