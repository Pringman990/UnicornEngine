#include "EnginePch.h"
#include "StackTrace.h"

#include <StackWalker/StackWalker.h>
#include <array>
#include <mutex>
#include <unordered_set>

constexpr std::size_t STACK_TRACE_MAX_LINES = 32;
constexpr std::size_t STACK_TRACE_MAX_LINE_LENGTH = 1024;

struct StackTraceImpl
{
	std::array<const char*, STACK_TRACE_MAX_LINES> myLines = {};
	bool operator==(const StackTraceImpl& aStackTraceImpl) const noexcept
	{
		return memcmp(myLines.data(), aStackTraceImpl.myLines.data(), sizeof(const char*) * STACK_TRACE_MAX_LINES) == 0;
	}
};


namespace std
{
	template<> struct hash<StackTraceImpl>
	{
		std::size_t operator()(const StackTraceImpl& aStackTrace) const noexcept
		{
			size_t result = 0;
			for (auto e : aStackTrace.myLines)
			{
				// Magic function to combine hashes. Should be switched if we add a dependency on a library with hash utility functions
				result ^= std::hash<const char*>{}(e)+0x9e3779b9 + (result << 6) + (result >> 2);
			}
			return result;
		}
	};
}

struct StackTraceLineHash
{
	size_t operator() (const std::array<char, STACK_TRACE_MAX_LINE_LENGTH>& line) const
	{
		return std::hash<std::string_view>()(std::string_view(line.data(), std::strlen(line.data())));
	}
};

class ToStringStackWalker : public StackWalker
{
public:
	ToStringStackWalker() : StackWalker()
	{
		LoadModules();
	}
	void SetBuffer(std::array<const char*, STACK_TRACE_MAX_LINES>* buffer, int skipCount = 0);
protected:
	virtual void OnOutput(LPCSTR szText);
	virtual void OnDbgHelpErr(LPCSTR, DWORD, DWORD64) {}; // supress error output
private:
	std::array<const char*, STACK_TRACE_MAX_LINES>* myBuffer = 0;
	int myCurrentIndex = 0;
};

static std::unordered_set<std::array<char, STACK_TRACE_MAX_LINE_LENGTH>, StackTraceLineHash> loStackTraceLineCache;
static std::unordered_set<StackTraceImpl> loStackTraceCache;
static ToStringStackWalker locStackWalker;
static std::mutex locStackWalkerMutex;

void ToStringStackWalker::SetBuffer(std::array<const char*, STACK_TRACE_MAX_LINES>* buffer, int skipCount)
{
	myBuffer = buffer;
	myCurrentIndex = -skipCount;
	m_MaxRecursionCount = STACK_TRACE_MAX_LINES + skipCount;
}

void ToStringStackWalker::OnOutput(LPCSTR szText)
{
	if (!myBuffer)
		return;

	if (myCurrentIndex < 0)
	{
		myCurrentIndex++;
		return;
	}

	if (myCurrentIndex >= STACK_TRACE_MAX_LINES)
		return;

	std::array<char, STACK_TRACE_MAX_LINE_LENGTH> myLine{};
	strncpy_s(myLine.data(), myLine.size(), szText, STACK_TRACE_MAX_LINE_LENGTH);
	auto pair = loStackTraceLineCache.insert(myLine);
	const char* cachedLine = pair.first->data();

	(*myBuffer)[myCurrentIndex] = cachedLine;
	myCurrentIndex++;
}

StackTrace::StackTrace(const StackTraceImpl& aStackTraceImpl) : myImpl(&aStackTraceImpl) {}

StackTrace StackTrace::CaptureStackTrace(int aSkipDepth)
{
	std::lock_guard<std::mutex> guard(locStackWalkerMutex);

	StackTraceImpl impl = {};
	locStackWalker.SetBuffer(&impl.myLines, 2 + aSkipDepth);
	locStackWalker.ShowCallstack();
	locStackWalker.SetBuffer(nullptr);
	auto it = loStackTraceCache.insert(impl).first;
	return StackTrace(*it);
}

void StackTrace::Print() const
{
	if (myImpl == nullptr)
	{
		OutputDebugStringA("Empty Stack Trace\n");
	}

	for (auto line : myImpl->myLines)
	{
		if (line == nullptr)
			break;
		OutputDebugStringA(line);
	}
}

std::size_t StackTrace::ComputeHash() const
{
	if (myImpl == nullptr)
		return 0;

	return std::hash<StackTraceImpl>()(*myImpl);
}