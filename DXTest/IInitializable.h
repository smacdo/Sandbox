#pragma once

// Represents a class that can be initialized. Code that cleans up and returns the class to a pre-initialized
// state should be put into Shutdown. The class destructor will call the Shutdown method if it is still initialized
// at destruction time. Any IInitializable should be able to cycle between being initialized and not initialized.
class IInitializable
{
public:
	IInitializable()
		: mInitialized(false)
	{
	}

	virtual ~IInitializable()
	{
		Shutdown();
	}

	// Shutdown the class, clean up and then reset the initialization flag to false.
	void Shutdown()
	{
		if (mInitialized)
		{
			OnShutdown();
			mInitialized = false;
		}
	}

	bool IsInitialized() const
	{
		return mInitialized;
	}

protected:
	void SetInitialized()
	{
		mInitialized = true;
	}

	virtual void OnShutdown()
	{

	}

private:
	bool mInitialized;
};

