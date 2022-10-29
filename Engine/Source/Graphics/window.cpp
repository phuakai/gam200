#include "window.h"


namespace window
{
	struct win
	{
	public:
		void winInit();
		void winUpdate();
		void winDestroy();
		int getWinWidth();
		int getWinHeight();

	private:
		int width;
		int height;
	};
}
