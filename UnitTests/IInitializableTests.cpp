#include "stdafx.h"
#include "CppUnitTest.h"
#include "IInitializable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(IInitializableTests)
    {
    private:
        class TestInitClass : public IInitializable
        {
        public:
            TestInitClass()
                : mTimesOnShutdownCalled(0),
                  mTimesOnInitializeCalled(0)
            {
            }

            void Start()
            {
                SetInitialized();
            }

            bool WasOnShutdownCalled() const { return mTimesOnShutdownCalled > 0; }
            bool WasOnInitializedCalled() const { return mTimesOnInitializeCalled > 0; }
            
            int TimesOnShutdownCalled() const { return mTimesOnShutdownCalled; }
            int TimesOnInitializedCalled() const { return mTimesOnInitializeCalled; }

            virtual void OnShutdown() override
            {
                mTimesOnShutdownCalled++;
            }

            virtual void OnInitialize() override
            {
                mTimesOnInitializeCalled++;
            }

        private:
            int mTimesOnShutdownCalled;
            int mTimesOnInitializeCalled;
        };

    public:
        TEST_METHOD(InitializableObjectStartsUninitialized)
        {
            TestInitClass obj;

            Assert::IsFalse(obj.IsInitialized());
            Assert::AreEqual(0, obj.TimesOnInitializedCalled());
            Assert::AreEqual(0, obj.TimesOnShutdownCalled());
        }

        TEST_METHOD(SetInitializedSetsInitializedToTrue)
        {
            TestInitClass obj;

            obj.Start();
            Assert::IsTrue(obj.IsInitialized());
        }

        TEST_METHOD(OnShutdownCalledAfterShutdown)
        {
            TestInitClass obj;

            obj.Start();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(0, obj.TimesOnShutdownCalled());

            obj.Shutdown();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(1, obj.TimesOnShutdownCalled());
        }

        TEST_METHOD(CanCallShutdownMoreThanOnceButDoesNothing)
        {
            TestInitClass obj;

            obj.Start();

            obj.Shutdown();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(1, obj.TimesOnShutdownCalled());

            obj.Shutdown();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(1, obj.TimesOnShutdownCalled());
        }

        TEST_METHOD(CanCallStartMoreThanOnceButDoesNothing)
        {
            TestInitClass obj;

            obj.Start();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(0, obj.TimesOnShutdownCalled());

            obj.Start();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(0, obj.TimesOnShutdownCalled());
        }

        TEST_METHOD(ShutdownBeforeStartDoesNothing)
        {
            TestInitClass obj;

            obj.Shutdown();
            Assert::AreEqual(0, obj.TimesOnInitializedCalled());
            Assert::AreEqual(0, obj.TimesOnShutdownCalled());
        }

        TEST_METHOD(CanRestartMoreThanOnce)
        {
            TestInitClass obj;

            obj.Start();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(0, obj.TimesOnShutdownCalled());

            obj.Shutdown();
            Assert::AreEqual(1, obj.TimesOnInitializedCalled());
            Assert::AreEqual(1, obj.TimesOnShutdownCalled());

            obj.Start();
            Assert::AreEqual(2, obj.TimesOnInitializedCalled());
            Assert::AreEqual(1, obj.TimesOnShutdownCalled());

            obj.Shutdown();
            Assert::AreEqual(2, obj.TimesOnInitializedCalled());
            Assert::AreEqual(2, obj.TimesOnShutdownCalled());
        }
    };
}