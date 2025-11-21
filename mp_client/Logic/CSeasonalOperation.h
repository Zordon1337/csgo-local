#include <vector>
#include <string>
// just to note here, im using DD/MM/YYYY for date
struct CSeason {
	int iId;
	int beginDate[3];
	int endDate[3];
};
namespace CSeasonalOperation {
	inline std::vector<CSeason> vSeasons = {};
	inline int iCurrSeason = -1;
	inline CSeason currSeason; // very confusing name but no idea for another
	void Init(int ver[3]);
	void UpdateMedals();
	std::string GetOperationName();
}