#pragma once

/**
 * Represents a class that can be initialized. Code that cleans up and returns the class to a pre-initialized
 * state should be put into Shutdown. The class destructor will call the Shutdown method if it is still initialized
 * at destruction time. Any IInitializable should be able to cycle between being initialized and not initialized.
 */
class IInitializable
{
public:
    IInitializable();
    virtual ~IInitializable();

    void Shutdown();
    bool IsInitialized() const;

protected:
    void SetInitialized();
    virtual void OnShutdown();
    virtual void OnInitialize();

private:
	bool mInitialized;
};

