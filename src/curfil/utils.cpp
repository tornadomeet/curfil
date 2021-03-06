#if 0
#######################################################################################
# The MIT License

# Copyright (c) 2014       Hannes Schulz, University of Bonn  <schulz@ais.uni-bonn.de>
# Copyright (c) 2013       Benedikt Waldvogel, University of Bonn <mail@bwaldvogel.de>
# Copyright (c) 2008-2009  Sebastian Nowozin                       <nowozin@gmail.com>

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#######################################################################################
#endif
#include "utils.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <cuda_runtime_api.h>

#include "version.h"

namespace curfil {

void logVersionInfo() {
#ifdef NDEBUG
    CURFIL_INFO("release version " << getVersion());
#else
    CURFIL_WARNING("this is the debugging version " << getVersion());
#endif
}

namespace utils {

bool Profile::enabled = false;

void Timer::reset() {
    stop();
    start();
}

void Timer::start() {
    started = boost::posix_time::microsec_clock::local_time();
    isStopped = false;
}
void Timer::stop() {
    stopped = boost::posix_time::microsec_clock::local_time();
    isStopped = true;
}

std::string Timer::format(int precision) {
    std::ostringstream o;
    o.precision(precision);
    if (getSeconds() <= 1.0) {
        o << std::fixed << getMilliseconds() << " ms";
    } else {
        o << std::fixed << getSeconds() << " s";
    }
    return o.str();
}

double Timer::getSeconds() {
    if (!isStopped)
        stop();
    boost::posix_time::time_duration duration = (stopped - started);
    return duration.total_microseconds() / static_cast<double>(1e6);
}

double Timer::getMilliseconds() {
    if (!isStopped)
        stop();
    boost::posix_time::time_duration duration = (stopped - started);
    return duration.total_microseconds() / static_cast<double>(1e3);
}

void logMessage(const std::string& msg, std::ostream& os) {
    boost::posix_time::ptime date_time = boost::posix_time::microsec_clock::local_time();

    std::string out = boost::str(boost::format("%s  %s") % date_time % msg);
    std::ostringstream endlStream;
    endlStream << std::endl;
    std::string endl = endlStream.str();

    // append newline if it’s not already there
    if (!boost::ends_with(out, endl)) {
        out += endl;
    }
    os << out << std::flush;
}

void checkCudaError(const char* msg) {
    cudaError_t lastError = cudaGetLastError();
    if (lastError == cudaSuccess)
        return;

    throw std::runtime_error(std::string(msg) + ": " + cudaGetErrorString(lastError));
}

size_t getFreeMemoryOnGPU(int deviceId) {
    size_t freeMemory = 0;
    size_t totalMemory = 0;
    cudaSafeCall(cudaSetDevice(deviceId));
    cudaSafeCall(cudaMemGetInfo(&freeMemory, &totalMemory));
    return freeMemory;
}

}
}
