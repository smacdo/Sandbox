#pragma once

/**
 * Represents a class that can be initialized. Code that cleans up and returns the class to a pre-initialized
 * state should be put into Shutdown. The class destructor will call the Shutdown method if it is still initialized
 * at destruction time. Any IInitializable should be able to cycle between being initialized and not initialized.
 *
 * A class using IInitializable should ensure they adhere to the following guidelines. When the class is correctly
 * initialized (for example, in its Start() routine), it should call SetInitialized() before returning. All code for
 * shutdown should be contained in OnShutdown() virtual method. This is always called by the destructor, but it can
 * also be called by the user manually.
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

