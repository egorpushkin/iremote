/**
 * Copyright (c) 2013 Egor Pushkin. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Common/Common.h"

#include <sys/sysctl.h>
#include "Processes.bsd.h"

namespace RemotePC
{

	/**
	 * This pretty routine was found at
	 * http://developer.apple.com/qa/qa2001/qa1123.html
	 * and it does exactly what it is intended for.
	 */
	int Processes::GetBSDProcessList(kinfo_proc **procList, size_t *procCount)
		// Returns a list of all BSD processes on the system.  This routine
		// allocates the list and puts it in *procList and a count of the
		// number of entries in *procCount.  You are responsible for freeing
		// this list (use "free" from System framework).
		// On success, the function returns 0.
		// On error, the function returns a BSD errno value.
	{
		int                 err;
		kinfo_proc *        result;
		bool                done;
		static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
		// Declaring name as const requires us to cast it when passing it to
		// sysctl because the prototype doesn't include the const modifier.
		size_t              length;

		assert( procList != NULL);
		assert(*procList == NULL);
		assert(procCount != NULL);

		*procCount = 0;

		// We start by calling sysctl with result == NULL and length == 0.
		// That will succeed, and set length to the appropriate length.
		// We then allocate a buffer of that size and call sysctl again
		// with that buffer.  If that succeeds, we're done.  If that fails
		// with ENOMEM, we have to throw away our buffer and loop.  Note
		// that the loop causes use to call sysctl with NULL again; this
		// is necessary because the ENOMEM failure case sets length to
		// the amount of data returned, not the amount of data that
		// could have been returned.

		result = NULL;
		done = false;
		do {
			assert(result == NULL);

			// Call sysctl with a NULL buffer.

			length = 0;
			err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
						  NULL, &length,
						  NULL, 0);
			if (err == -1) {
				err = errno;
			}

			// Allocate an appropriately sized buffer based on the results
			// from the previous call.

			if (err == 0) {
				result = (kinfo_proc *)malloc(length);
				if (result == NULL) {
					err = ENOMEM;
				}
			}

			// Call sysctl again with the new buffer.  If we get an ENOMEM
			// error, toss away our buffer and start again.

			if (err == 0) {
				err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
							  result, &length,
							  NULL, 0);
				if (err == -1) {
					err = errno;
				}
				if (err == 0) {
					done = true;
				} else if (err == ENOMEM) {
					assert(result != NULL);
					free(result);
					result = NULL;
					err = 0;
				}
			}
		} while (err == 0 && ! done);

		// Clean up and establish post conditions.

		if (err != 0 && result != NULL) {
			free(result);
			result = NULL;
		}
		*procList = result;
		if (err == 0) {
			*procCount = length / sizeof(kinfo_proc);
		}

		assert( (err == 0) == (*procList != NULL) );

		return err;
	}

	void Processes::Free(kinfo_proc *procList)
	{
		assert(procList != NULL);
		free(procList);
	}

	pid_t Processes::UnwrapPid(kinfo_proc *process)
	{
		assert(process != NULL);
		return process->kp_proc.p_pid;
	}

	pid_t Processes::UnwrapParentPid(kinfo_proc *process)
	{
		assert(process != NULL);
		return process->kp_eproc.e_ppid;
	}

}
