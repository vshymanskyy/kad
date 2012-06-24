/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>

bool suite_LogTS_init = false;
#include "./xplat/test/XLogTS.h"

static LogTS suite_LogTS;

static CxxTest::List Tests_LogTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LogTS( "./xplat/test/XLogTS.h", 9, "LogTS", suite_LogTS, Tests_LogTS );

static class TestDescription_suite_LogTS_testLog : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_LogTS_testLog() : CxxTest::RealTestDescription( Tests_LogTS, suiteDescription_LogTS, 36, "testLog" ) {}
 void runTest() { suite_LogTS.testLog(); }
} testDescription_suite_LogTS_testLog;

#include "./xplat/test/XPlatTS.h"

static XPlatTS suite_XPlatTS;

static CxxTest::List Tests_XPlatTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_XPlatTS( "./xplat/test/XPlatTS.h", 18, "XPlatTS", suite_XPlatTS, Tests_XPlatTS );

static class TestDescription_suite_XPlatTS_testEndianness : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XPlatTS_testEndianness() : CxxTest::RealTestDescription( Tests_XPlatTS, suiteDescription_XPlatTS, 21, "testEndianness" ) {}
 void runTest() { suite_XPlatTS.testEndianness(); }
} testDescription_suite_XPlatTS_testEndianness;

#include "./xplat/test/net/XSocketUdpTS.h"

static XSocketUdpTS suite_XSocketUdpTS;

static CxxTest::List Tests_XSocketUdpTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_XSocketUdpTS( "./xplat/test/net/XSocketUdpTS.h", 4, "XSocketUdpTS", suite_XSocketUdpTS, Tests_XSocketUdpTS );

static class TestDescription_suite_XSocketUdpTS_testBindLocal : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSocketUdpTS_testBindLocal() : CxxTest::RealTestDescription( Tests_XSocketUdpTS, suiteDescription_XSocketUdpTS, 8, "testBindLocal" ) {}
 void runTest() { suite_XSocketUdpTS.testBindLocal(); }
} testDescription_suite_XSocketUdpTS_testBindLocal;

static class TestDescription_suite_XSocketUdpTS_testBindLocal2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSocketUdpTS_testBindLocal2() : CxxTest::RealTestDescription( Tests_XSocketUdpTS, suiteDescription_XSocketUdpTS, 29, "testBindLocal2" ) {}
 void runTest() { suite_XSocketUdpTS.testBindLocal2(); }
} testDescription_suite_XSocketUdpTS_testBindLocal2;

static class TestDescription_suite_XSocketUdpTS_testLoopback4 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSocketUdpTS_testLoopback4() : CxxTest::RealTestDescription( Tests_XSocketUdpTS, suiteDescription_XSocketUdpTS, 34, "testLoopback4" ) {}
 void runTest() { suite_XSocketUdpTS.testLoopback4(); }
} testDescription_suite_XSocketUdpTS_testLoopback4;

static class TestDescription_suite_XSocketUdpTS_testLoopback6 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSocketUdpTS_testLoopback6() : CxxTest::RealTestDescription( Tests_XSocketUdpTS, suiteDescription_XSocketUdpTS, 64, "testLoopback6" ) {}
 void runTest() { suite_XSocketUdpTS.testLoopback6(); }
} testDescription_suite_XSocketUdpTS_testLoopback6;

#include "./xplat/test/net/XAddrTS.h"

static XSockAddrTS suite_XSockAddrTS;

static CxxTest::List Tests_XSockAddrTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_XSockAddrTS( "./xplat/test/net/XAddrTS.h", 4, "XSockAddrTS", suite_XSockAddrTS, Tests_XSockAddrTS );

static class TestDescription_suite_XSockAddrTS_testGoodAddrString : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSockAddrTS_testGoodAddrString() : CxxTest::RealTestDescription( Tests_XSockAddrTS, suiteDescription_XSockAddrTS, 8, "testGoodAddrString" ) {}
 void runTest() { suite_XSockAddrTS.testGoodAddrString(); }
} testDescription_suite_XSockAddrTS_testGoodAddrString;

static class TestDescription_suite_XSockAddrTS_testBadAddrString : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSockAddrTS_testBadAddrString() : CxxTest::RealTestDescription( Tests_XSockAddrTS, suiteDescription_XSockAddrTS, 44, "testBadAddrString" ) {}
 void runTest() { suite_XSockAddrTS.testBadAddrString(); }
} testDescription_suite_XSockAddrTS_testBadAddrString;

static class TestDescription_suite_XSockAddrTS_testAltAddrString : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSockAddrTS_testAltAddrString() : CxxTest::RealTestDescription( Tests_XSockAddrTS, suiteDescription_XSockAddrTS, 63, "testAltAddrString" ) {}
 void runTest() { suite_XSockAddrTS.testAltAddrString(); }
} testDescription_suite_XSockAddrTS_testAltAddrString;

static class TestDescription_suite_XSockAddrTS_testLookup : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSockAddrTS_testLookup() : CxxTest::RealTestDescription( Tests_XSockAddrTS, suiteDescription_XSockAddrTS, 87, "testLookup" ) {}
 void runTest() { suite_XSockAddrTS.testLookup(); }
} testDescription_suite_XSockAddrTS_testLookup;

static class TestDescription_suite_XSockAddrTS_testResolve : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSockAddrTS_testResolve() : CxxTest::RealTestDescription( Tests_XSockAddrTS, suiteDescription_XSockAddrTS, 92, "testResolve" ) {}
 void runTest() { suite_XSockAddrTS.testResolve(); }
} testDescription_suite_XSockAddrTS_testResolve;

static class TestDescription_suite_XSockAddrTS_testLocal : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_XSockAddrTS_testLocal() : CxxTest::RealTestDescription( Tests_XSockAddrTS, suiteDescription_XSockAddrTS, 108, "testLocal" ) {}
 void runTest() { suite_XSockAddrTS.testLocal(); }
} testDescription_suite_XSockAddrTS_testLocal;

#include "./xplat/test/XLogUtilsTS.h"

static LogUtilsTS suite_LogUtilsTS;

static CxxTest::List Tests_LogUtilsTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LogUtilsTS( "./xplat/test/XLogUtilsTS.h", 5, "LogUtilsTS", suite_LogUtilsTS, Tests_LogUtilsTS );

static class TestDescription_suite_LogUtilsTS_testLogDump : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_LogUtilsTS_testLogDump() : CxxTest::RealTestDescription( Tests_LogUtilsTS, suiteDescription_LogUtilsTS, 9, "testLogDump" ) {}
 void runTest() { suite_LogUtilsTS.testLogDump(); }
} testDescription_suite_LogUtilsTS_testLogDump;

#include "./xplat/test/XListTS.h"

static ListTS suite_ListTS;

static CxxTest::List Tests_ListTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ListTS( "./xplat/test/XListTS.h", 4, "ListTS", suite_ListTS, Tests_ListTS );

static class TestDescription_suite_ListTS_testFirstAndLast : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testFirstAndLast() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 8, "testFirstAndLast" ) {}
 void runTest() { suite_ListTS.testFirstAndLast(); }
} testDescription_suite_ListTS_testFirstAndLast;

static class TestDescription_suite_ListTS_testIteratorOffsets : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testIteratorOffsets() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 25, "testIteratorOffsets" ) {}
 void runTest() { suite_ListTS.testIteratorOffsets(); }
} testDescription_suite_ListTS_testIteratorOffsets;

static class TestDescription_suite_ListTS_testIteratorOutOfBounts : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testIteratorOutOfBounts() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 42, "testIteratorOutOfBounts" ) {}
 void runTest() { suite_ListTS.testIteratorOutOfBounts(); }
} testDescription_suite_ListTS_testIteratorOutOfBounts;

static class TestDescription_suite_ListTS_testAppendPrepend : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testAppendPrepend() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 61, "testAppendPrepend" ) {}
 void runTest() { suite_ListTS.testAppendPrepend(); }
} testDescription_suite_ListTS_testAppendPrepend;

static class TestDescription_suite_ListTS_testInsertBeforeAfter : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testInsertBeforeAfter() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 78, "testInsertBeforeAfter" ) {}
 void runTest() { suite_ListTS.testInsertBeforeAfter(); }
} testDescription_suite_ListTS_testInsertBeforeAfter;

static class TestDescription_suite_ListTS_testRemoveInCycle : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testRemoveInCycle() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 96, "testRemoveInCycle" ) {}
 void runTest() { suite_ListTS.testRemoveInCycle(); }
} testDescription_suite_ListTS_testRemoveInCycle;

static class TestDescription_suite_ListTS_testReplaceInCycle : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testReplaceInCycle() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 120, "testReplaceInCycle" ) {}
 void runTest() { suite_ListTS.testReplaceInCycle(); }
} testDescription_suite_ListTS_testReplaceInCycle;

static class TestDescription_suite_ListTS_testFindFirstLast : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testFindFirstLast() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 162, "testFindFirstLast" ) {}
 void runTest() { suite_ListTS.testFindFirstLast(); }
} testDescription_suite_ListTS_testFindFirstLast;

static class TestDescription_suite_ListTS_testReverse : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testReverse() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 185, "testReverse" ) {}
 void runTest() { suite_ListTS.testReverse(); }
} testDescription_suite_ListTS_testReverse;

static class TestDescription_suite_ListTS_testSwapLinks : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testSwapLinks() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 220, "testSwapLinks" ) {}
 void runTest() { suite_ListTS.testSwapLinks(); }
} testDescription_suite_ListTS_testSwapLinks;

static class TestDescription_suite_ListTS_testCRTP : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ListTS_testCRTP() : CxxTest::RealTestDescription( Tests_ListTS, suiteDescription_ListTS, 258, "testCRTP" ) {}
 void runTest() { suite_ListTS.testCRTP(); }
} testDescription_suite_ListTS_testCRTP;

#include "./xplat/test/XThreadTS.h"

static ThreadTS suite_ThreadTS;

static CxxTest::List Tests_ThreadTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ThreadTS( "./xplat/test/XThreadTS.h", 5, "ThreadTS", suite_ThreadTS, Tests_ThreadTS );

static class TestDescription_suite_ThreadTS_testThread : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ThreadTS_testThread() : CxxTest::RealTestDescription( Tests_ThreadTS, suiteDescription_ThreadTS, 32, "testThread" ) {}
 void runTest() { suite_ThreadTS.testThread(); }
} testDescription_suite_ThreadTS_testThread;

static class TestDescription_suite_ThreadTS_testThreadId : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ThreadTS_testThreadId() : CxxTest::RealTestDescription( Tests_ThreadTS, suiteDescription_ThreadTS, 53, "testThreadId" ) {}
 void runTest() { suite_ThreadTS.testThreadId(); }
} testDescription_suite_ThreadTS_testThreadId;

#include "./xplat/test/XStringTS.h"

static StringTS suite_StringTS;

static CxxTest::List Tests_StringTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_StringTS( "./xplat/test/XStringTS.h", 4, "StringTS", suite_StringTS, Tests_StringTS );

static class TestDescription_suite_StringTS_testFormat : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testFormat() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 7, "testFormat" ) {}
 void runTest() { suite_StringTS.testFormat(); }
} testDescription_suite_StringTS_testFormat;

static class TestDescription_suite_StringTS_testCompare : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testCompare() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 16, "testCompare" ) {}
 void runTest() { suite_StringTS.testCompare(); }
} testDescription_suite_StringTS_testCompare;

static class TestDescription_suite_StringTS_testArray : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testArray() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 33, "testArray" ) {}
 void runTest() { suite_StringTS.testArray(); }
} testDescription_suite_StringTS_testArray;

static class TestDescription_suite_StringTS_testAppend : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testAppend() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 45, "testAppend" ) {}
 void runTest() { suite_StringTS.testAppend(); }
} testDescription_suite_StringTS_testAppend;

static class TestDescription_suite_StringTS_testStartsEndsWith : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testStartsEndsWith() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 53, "testStartsEndsWith" ) {}
 void runTest() { suite_StringTS.testStartsEndsWith(); }
} testDescription_suite_StringTS_testStartsEndsWith;

static class TestDescription_suite_StringTS_testLength : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testLength() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 71, "testLength" ) {}
 void runTest() { suite_StringTS.testLength(); }
} testDescription_suite_StringTS_testLength;

static class TestDescription_suite_StringTS_testSubstring : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testSubstring() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 91, "testSubstring" ) {}
 void runTest() { suite_StringTS.testSubstring(); }
} testDescription_suite_StringTS_testSubstring;

static class TestDescription_suite_StringTS_testFind : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_StringTS_testFind() : CxxTest::RealTestDescription( Tests_StringTS, suiteDescription_StringTS, 105, "testFind" ) {}
 void runTest() { suite_StringTS.testFind(); }
} testDescription_suite_StringTS_testFind;

#include "./xplat/test/XCompilerTS.h"

static CompilerRvoTS suite_CompilerRvoTS;

static CxxTest::List Tests_CompilerRvoTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CompilerRvoTS( "./xplat/test/XCompilerTS.h", 5, "CompilerRvoTS", suite_CompilerRvoTS, Tests_CompilerRvoTS );

static class TestDescription_suite_CompilerRvoTS_testRvo : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_CompilerRvoTS_testRvo() : CxxTest::RealTestDescription( Tests_CompilerRvoTS, suiteDescription_CompilerRvoTS, 19, "testRvo" ) {}
 void runTest() { suite_CompilerRvoTS.testRvo(); }
} testDescription_suite_CompilerRvoTS_testRvo;

#include "./xplat/test/XHelpersTS.h"

static HelpersTS suite_HelpersTS;

static CxxTest::List Tests_HelpersTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_HelpersTS( "./xplat/test/XHelpersTS.h", 4, "HelpersTS", suite_HelpersTS, Tests_HelpersTS );

static class TestDescription_suite_HelpersTS_testMin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HelpersTS_testMin() : CxxTest::RealTestDescription( Tests_HelpersTS, suiteDescription_HelpersTS, 7, "testMin" ) {}
 void runTest() { suite_HelpersTS.testMin(); }
} testDescription_suite_HelpersTS_testMin;

static class TestDescription_suite_HelpersTS_testMax : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HelpersTS_testMax() : CxxTest::RealTestDescription( Tests_HelpersTS, suiteDescription_HelpersTS, 22, "testMax" ) {}
 void runTest() { suite_HelpersTS.testMax(); }
} testDescription_suite_HelpersTS_testMax;

static class TestDescription_suite_HelpersTS_testSwap : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HelpersTS_testSwap() : CxxTest::RealTestDescription( Tests_HelpersTS, suiteDescription_HelpersTS, 37, "testSwap" ) {}
 void runTest() { suite_HelpersTS.testSwap(); }
} testDescription_suite_HelpersTS_testSwap;

static class TestDescription_suite_HelpersTS_testInRange : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HelpersTS_testInRange() : CxxTest::RealTestDescription( Tests_HelpersTS, suiteDescription_HelpersTS, 44, "testInRange" ) {}
 void runTest() { suite_HelpersTS.testInRange(); }
} testDescription_suite_HelpersTS_testInRange;

static class TestDescription_suite_HelpersTS_testOutOfRange : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HelpersTS_testOutOfRange() : CxxTest::RealTestDescription( Tests_HelpersTS, suiteDescription_HelpersTS, 54, "testOutOfRange" ) {}
 void runTest() { suite_HelpersTS.testOutOfRange(); }
} testDescription_suite_HelpersTS_testOutOfRange;

static class TestDescription_suite_HelpersTS_testAbsDiff : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HelpersTS_testAbsDiff() : CxxTest::RealTestDescription( Tests_HelpersTS, suiteDescription_HelpersTS, 64, "testAbsDiff" ) {}
 void runTest() { suite_HelpersTS.testAbsDiff(); }
} testDescription_suite_HelpersTS_testAbsDiff;

#include "./xplat/test/XTimingTS.h"

static TimingTS suite_TimingTS;

static CxxTest::List Tests_TimingTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TimingTS( "./xplat/test/XTimingTS.h", 5, "TimingTS", suite_TimingTS, Tests_TimingTS );

static class TestDescription_suite_TimingTS_testSleep : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testSleep() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 58, "testSleep" ) {}
 void runTest() { suite_TimingTS.testSleep(); }
} testDescription_suite_TimingTS_testSleep;

static class TestDescription_suite_TimingTS_testTimer : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimer() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 71, "testTimer" ) {}
 void runTest() { suite_TimingTS.testTimer(); }
} testDescription_suite_TimingTS_testTimer;

static class TestDescription_suite_TimingTS_testTimerCancel : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimerCancel() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 86, "testTimerCancel" ) {}
 void runTest() { suite_TimingTS.testTimerCancel(); }
} testDescription_suite_TimingTS_testTimerCancel;

static class TestDescription_suite_TimingTS_testTimerCancel2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimerCancel2() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 104, "testTimerCancel2" ) {}
 void runTest() { suite_TimingTS.testTimerCancel2(); }
} testDescription_suite_TimingTS_testTimerCancel2;

static class TestDescription_suite_TimingTS_testTimerCancelInHandler : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimerCancelInHandler() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 120, "testTimerCancelInHandler" ) {}
 void runTest() { suite_TimingTS.testTimerCancelInHandler(); }
} testDescription_suite_TimingTS_testTimerCancelInHandler;

static class TestDescription_suite_TimingTS_testTimerMultiple : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimerMultiple() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 129, "testTimerMultiple" ) {}
 void runTest() { suite_TimingTS.testTimerMultiple(); }
} testDescription_suite_TimingTS_testTimerMultiple;

static class TestDescription_suite_TimingTS_testTimerRepeats : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimerRepeats() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 155, "testTimerRepeats" ) {}
 void runTest() { suite_TimingTS.testTimerRepeats(); }
} testDescription_suite_TimingTS_testTimerRepeats;

static class TestDescription_suite_TimingTS_testTimerDrift : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TimingTS_testTimerDrift() : CxxTest::RealTestDescription( Tests_TimingTS, suiteDescription_TimingTS, 191, "testTimerDrift" ) {}
 void runTest() { suite_TimingTS.testTimerDrift(); }
} testDescription_suite_TimingTS_testTimerDrift;

#include "./src/test/KadConnMgrTS.h"

static KadConnMgrTS suite_KadConnMgrTS;

static CxxTest::List Tests_KadConnMgrTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_KadConnMgrTS( "./src/test/KadConnMgrTS.h", 4, "KadConnMgrTS", suite_KadConnMgrTS, Tests_KadConnMgrTS );

static class TestDescription_suite_KadConnMgrTS_testPing : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadConnMgrTS_testPing() : CxxTest::RealTestDescription( Tests_KadConnMgrTS, suiteDescription_KadConnMgrTS, 8, "testPing" ) {}
 void runTest() { suite_KadConnMgrTS.testPing(); }
} testDescription_suite_KadConnMgrTS_testPing;

#include "./src/test/KademliaTS.h"

static KademliaTS suite_KademliaTS;

static CxxTest::List Tests_KademliaTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_KademliaTS( "./src/test/KademliaTS.h", 4, "KademliaTS", suite_KademliaTS, Tests_KademliaTS );

static class TestDescription_suite_KademliaTS_testRouting1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KademliaTS_testRouting1() : CxxTest::RealTestDescription( Tests_KademliaTS, suiteDescription_KademliaTS, 7, "testRouting1" ) {}
 void runTest() { suite_KademliaTS.testRouting1(); }
} testDescription_suite_KademliaTS_testRouting1;

#include "./src/test/KadOpMgrTS.h"

static KadOpMgrTS suite_KadOpMgrTS;

static CxxTest::List Tests_KadOpMgrTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_KadOpMgrTS( "./src/test/KadOpMgrTS.h", 4, "KadOpMgrTS", suite_KadOpMgrTS, Tests_KadOpMgrTS );

static class TestDescription_suite_KadOpMgrTS_testPing : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadOpMgrTS_testPing() : CxxTest::RealTestDescription( Tests_KadOpMgrTS, suiteDescription_KadOpMgrTS, 61, "testPing" ) {}
 void runTest() { suite_KadOpMgrTS.testPing(); }
} testDescription_suite_KadOpMgrTS_testPing;

static class TestDescription_suite_KadOpMgrTS_testJoin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadOpMgrTS_testJoin() : CxxTest::RealTestDescription( Tests_KadOpMgrTS, suiteDescription_KadOpMgrTS, 67, "testJoin" ) {}
 void runTest() { suite_KadOpMgrTS.testJoin(); }
} testDescription_suite_KadOpMgrTS_testJoin;

#include "./src/test/KadNodeIdTS.h"

static KadNodeIdTS suite_KadNodeIdTS;

static CxxTest::List Tests_KadNodeIdTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_KadNodeIdTS( "./src/test/KadNodeIdTS.h", 4, "KadNodeIdTS", suite_KadNodeIdTS, Tests_KadNodeIdTS );

static class TestDescription_suite_KadNodeIdTS_testFromHex : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadNodeIdTS_testFromHex() : CxxTest::RealTestDescription( Tests_KadNodeIdTS, suiteDescription_KadNodeIdTS, 8, "testFromHex" ) {}
 void runTest() { suite_KadNodeIdTS.testFromHex(); }
} testDescription_suite_KadNodeIdTS_testFromHex;

static class TestDescription_suite_KadNodeIdTS_testOperations : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadNodeIdTS_testOperations() : CxxTest::RealTestDescription( Tests_KadNodeIdTS, suiteDescription_KadNodeIdTS, 29, "testOperations" ) {}
 void runTest() { suite_KadNodeIdTS.testOperations(); }
} testDescription_suite_KadNodeIdTS_testOperations;

static class TestDescription_suite_KadNodeIdTS_testCompare : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadNodeIdTS_testCompare() : CxxTest::RealTestDescription( Tests_KadNodeIdTS, suiteDescription_KadNodeIdTS, 45, "testCompare" ) {}
 void runTest() { suite_KadNodeIdTS.testCompare(); }
} testDescription_suite_KadNodeIdTS_testCompare;

static class TestDescription_suite_KadNodeIdTS_testDistanceTo : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadNodeIdTS_testDistanceTo() : CxxTest::RealTestDescription( Tests_KadNodeIdTS, suiteDescription_KadNodeIdTS, 64, "testDistanceTo" ) {}
 void runTest() { suite_KadNodeIdTS.testDistanceTo(); }
} testDescription_suite_KadNodeIdTS_testDistanceTo;

static class TestDescription_suite_KadNodeIdTS_testCloser : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_KadNodeIdTS_testCloser() : CxxTest::RealTestDescription( Tests_KadNodeIdTS, suiteDescription_KadNodeIdTS, 84, "testCloser" ) {}
 void runTest() { suite_KadNodeIdTS.testCloser(); }
} testDescription_suite_KadNodeIdTS_testCloser;

#include "./src/test/ShaTS.h"

static ShaTS suite_ShaTS;

static CxxTest::List Tests_ShaTS = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ShaTS( "./src/test/ShaTS.h", 24, "ShaTS", suite_ShaTS, Tests_ShaTS );

static class TestDescription_suite_ShaTS_testSha1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ShaTS_testSha1() : CxxTest::RealTestDescription( Tests_ShaTS, suiteDescription_ShaTS, 28, "testSha1" ) {}
 void runTest() { suite_ShaTS.testSha1(); }
} testDescription_suite_ShaTS_testSha1;

static class TestDescription_suite_ShaTS_testSha256 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ShaTS_testSha256() : CxxTest::RealTestDescription( Tests_ShaTS, suiteDescription_ShaTS, 39, "testSha256" ) {}
 void runTest() { suite_ShaTS.testSha256(); }
} testDescription_suite_ShaTS_testSha256;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
