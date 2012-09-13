// go_client - Go client to access the kernel mmap area
//
// Written in 2012 by Prashant P Shah <pshah.mumbai@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated
// all copyright and related and neighboring rights to this software
// to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication
// along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

package main

import (
	"fmt"
	"os"
	"syscall"
	"unsafe"
)

const device_file = "./mmapdev"  // CHANGE THIS

func main() {
	const n = 4096 * 1000	// size of shared memory
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

	mmap[0] = 20	// Writing data to first byte of shared memory
	mmap_array := (*[n]byte)(unsafe.Pointer(&mmap[0]))

	fmt.Println(*mmap_array)

	// using memcpy to copy shared memory buffer into program buffer
	mmap_buf := make([]byte, t)
	copy(mmap_buf, mmap_array[:])
	fmt.Println(mmap_buf)
}
