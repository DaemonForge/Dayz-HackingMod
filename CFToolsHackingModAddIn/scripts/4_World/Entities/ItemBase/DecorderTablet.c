class DecoderTablet extends ItemBase{
	void HackCompleted(ItemBase HackingTarget, PlayerBase Hacker){
		super.HackCompleted( HackingTarget, Hacker)
		PlayerBase hacker = PlayerBase.Cast(Hacker);
		ItemBase hackingTarget = ItemBase.Cast(HackingTarget);
        if (hackingTarget && hacker && m_HackingCompleted) {

			CFAPI api = GetDayZGame().GetCFAPI();
			string request_json, response_raw, error;

			ItemDestroyedData request = new ItemDestroyedData(api.get_config());
			request.update(player, hackingTarget.GetDisplayName(), this.GetDisplayName());

			api.json.WriteToString(request, false, request_json);

			api.get_context().POST(new CFAPIItemHacked, api.build_url("item", "destroyed", "1"), request_json);
        }
	}
	
};