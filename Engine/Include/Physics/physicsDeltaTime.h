#pragma once

#include <GLFW/glfw3.h>

//#if false

namespace physicsDeltaTime
{
	template <typename T>
	struct dt
	{
	public:
		//default constructor
		dt();

		void updateDT();
		int getNoOfSteps();
		T getDeltaTime();
		void getDeltaTime(T& inputDt);

	private:
		int currNoOfSteps;
		double prevTime;
		double currTime;
		double deltaTime;
		double accumulatedTime;
		const double fixedDT = 1.0 / 60.0;
	};

	//dt physicsDT; //singleton
	template <typename T>
	dt<T>::dt()
	{
		prevTime = glfwGetTime();
		currTime = accumulatedTime = currNoOfSteps = 0.0;
	}

	template <typename T>
	int dt<T>::getNoOfSteps()
	{
		return currNoOfSteps;
	}

	template <typename T>
	T dt<T>::getDeltaTime()
	{
		return static_cast<T>(deltaTime);
	}

	template <typename T>
	void dt<T>::getDeltaTime(T& inputDt)
	{
		inputDt = static_cast<T>(deltaTime);
	}

	template <typename T>
	void dt<T>::updateDT()
	{
		currTime = glfwGetTime();

		currNoOfSteps = 0;						// reset

		deltaTime = currTime - prevTime;		// compute the actual game loop time
		prevTime = currTime;
		accumulatedTime += deltaTime;			//adding the actual game loop time

		while (accumulatedTime >= fixedDT)
		{
			accumulatedTime -= fixedDT;			//this will save the exact accumulated time differences, among all game loops
			++currNoOfSteps;
		}
	}

}

//#endif