let exp = require("express");
let app = exp();

/*
struct CItem {
	int iDefIdx;
	int iRarity;
	float flPaintKit;
	bool bHasStattrack;
	float flStattrack;
	long iItemId;
	float flWear;
	int iPattern;
	int iQuality;
	int iFlag;
};
*/
let userEquips = [
    { userId: 0, defIdx: 0, slotId: 0, teamId: 0, paintKit: 0, hasStattack: false, stattrack: -1, wear: 0, pattern: 0, quality: 0 },
]

app.get("/get_user_equips", (req, res) => {
    let userId = parseInt(req.query.userId);
    let equips = userEquips.filter(e => e.userId === userId);
    res.json(equips);
});

app.post("/set_user_equip", exp.json(), (req, res) => {
    let equip = req.body;
    let index = userEquips.findIndex(e => e.userId === equip.userId && e.slotId === equip.slotId);
    if (index !== -1) {
        userEquips[index] = equip;
    } else {
        userEquips.push(equip);
    }
    res.json({ success: true });
});


app.listen(3000, () => {
    console.log("Server is running on port 3000");
})