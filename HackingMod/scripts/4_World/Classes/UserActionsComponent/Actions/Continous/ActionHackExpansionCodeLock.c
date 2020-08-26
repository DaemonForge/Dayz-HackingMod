class ActionHackCodeLockCB extends ActionContinuousBaseCB {
    override void CreateActionComponent() {
		float circleTime = 60; 
		if (m_ActionData)
		{		
			if (m_ActionData.m_Target)
			{
				ItemBase targetObject = ItemBase.Cast(m_ActionData.m_Target.GetObject());
				if (targetObject){
					
				}
			}
		}
      	m_ActionData.m_ActionComponent = new CAContinuousRepeat(circleTime);
    }
};

class ActionHackCodeLock : ActionContinuousBase {

	bool continueHack = false;
    void ActionHackCodeLockOnDoor() {
        m_CallbackClass = ActionHackCodeLockOnDoorCB;
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
   		ItemBase hacking_target;
		DecoderTablet tablet = DecoderTablet.Cast(item);
		if (Class.CastTo(hacking_target, target.GetObject()) || Class.CastTo(hacking_target, target.GetParent()) ){
			CodeLock codelock = CodeLock.Cast(hacking_target.GetAttachmentByConfigTypeName("CodeLock"));
			if (codelock && tablet){
				if (codelock.GetLockState() && tablet.WasHackingInterrupted() && tablet.ECLE_GetHackID() == hacking_target.ECLE_GetHackID()) {
					continueHack = true;
					if ( tablet.CountBatteries() >= GetHackingModConfig().) {
						return true;
					}
				}else if (codelock.GetLockState() && (!tablet.HasHackingStarted() || tablet.WasHackingInterrupted())) {
					continueHack = false;
					if ( tablet.CountBatteries() >= GetHackingModConfig().) {
						return true;
					}
				}
			}
		}
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data) {

        ItemBase target = ItemBase.Cast(action_data.m_Target.GetObject());    
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