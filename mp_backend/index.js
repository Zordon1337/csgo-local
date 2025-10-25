let exp = require("express");
let app = exp();


let userEquips = [
    { userId: 0, defIdx: 0, slotId: 0, teamId: 0, paintKit: 0, hasStattack: false, stattrack: -1, wear: 0, pattern: 0, quality: 0 },
]
let userProfiles = [

    { userId: 1, mmrank: 0, wmrank: 0, mmwins: 0, wmwins: 0, lvl: 1, xp: 0, medals: [9174] },
]

app.post("/set_user_profile", exp.json(), (req, res) => {
    let profile = req.body;
    
    console.log("Set profile:", profile);
    let index = userProfiles.findIndex(p => p.userId === profile.userId);
    if (index !== -1) {
        userProfiles[index] = profile;
    } else {
        userProfiles.push(profile);
    }
    res.json({ success: true });
});

app.get("/get_user_profile", (req, res) => {
    let userId = parseInt(req.query.userId);
    let equips = userProfiles.filter(e => e.userId === userId);
    console.log(`Get profile for user ${userId}:`, equips);
    res.json(equips);
});
app.get("/get_user_equips", (req, res) => {
    let userId = parseInt(req.query.userId);
    let equips = userEquips.filter(e => e.userId === userId);
    console.log(`Get equips for user ${userId}:`, equips);
    res.json(equips);
});

app.post("/set_user_equip", exp.json(), (req, res) => {
    let equip = req.body;
    console.log("Set equip:", equip);
    let index = userEquips.findIndex(e => e.userId === equip.userId && e.slotId === equip.slotId && e.teamId === equip.teamId);
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