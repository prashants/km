// go_client - Go client to access the kernel mmap area
//
// Written in 2012 by Prashant P Shah <pshah.mumbai@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

package main

import (
	"fmt"
	"os"
	"syscall"
	"unsafe"
)

const device_file = "./mmapdev" // CHANGE THIS

func main() {
	const n = 4096 * 50 // size of shared memory
	t := int(1) * n
	fmt.Println("Total size of shared memory : ", t)

	// open device file
	mmap_file, err := os.OpenFile(device_file, os.O_RDWR, 0666)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer mmap_file.Close()

	// mmap memory
	mmap, err := syscall.Mmap(int(mmap_file.Fd()), 0, int(t),
		syscall.PROT_READ|syscall.PROT_WRITE,
		syscall.MAP_SHARED)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer syscall.Munmap(mmap)

	mmap_array := (*[n]byte)(unsafe.Pointer(&mmap[0]))
	fmt.Println(*mmap_array)

	// using memcpy to copy mmap memory buffer into program buffer
	mmap_buf := make([]byte, t)
	copy(mmap_buf, mmap_array[:])
	fmt.Println(mmap_buf)
}
