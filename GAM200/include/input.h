
namespace Graphics
{
	class Input // Input class
	{       
		public:
			// Get cursor position in viewspace
			static bool getCursorPosViewspace(double* xpos, double* ypos); // Static allows function to be called without Input class object

			// Get cursor position, returned via address in input parameters xpos and ypos
			// Example usage : getCursorPos(&PositionX, &PositionY);
			static bool getCursorPos(double* xpos, double* ypos); // Static allows function to be called without Input class object
			
		private:            
			

	};
}