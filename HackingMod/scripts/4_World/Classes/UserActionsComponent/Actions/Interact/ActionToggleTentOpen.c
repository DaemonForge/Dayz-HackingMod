modded class ActionToggleTentOpen : ActionInteractBase
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) {
        TentBase tent = TentBase.Cast(target.GetParent());
        if (tent) {
            if ( tent.IsLocked()  && GetHackingModConfig().AllowCodeLocksOnTents ) { 
				return false; 
			}
        }
        return super.ActionCondition( player, target, item );
    }
	
	override void OnExecuteServer( ActionData action_data )
	{
		super.OnExecuteServer( action_data );
		Object targetObject = action_data.m_Target.GetObject();
		TentBase tent = TentBase.Cast( action_data.m_Target.GetParent() );
		if( tent && targetObject)
		{
			array<string> selections = new array<string>;
			targetObject.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
			for(int i = 0; i < selections.Count(); i++){
				tent.ToggleTentOpening(selections.Get(i));
			}
		}
	}
};
