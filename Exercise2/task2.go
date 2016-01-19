// Go 1.2
// go run helloworld_go.go

package main

import (
    . "fmt"
    "runtime"
 //   "time"
)
var i int = 0



func main() {
    runtime.GOMAXPROCS(runtime.NumCPU())    // I guess this is a hint to what GOMAXPROCS does...
                                           // Try doing the exercise both with and without it!
        c1 := make(chan int)
        c2 := make(chan int) 
        c3 := make(chan int)
        stop :=0                                    
        go func()  {   
         for j := 0; j < 1000000; j++ {
		c1 <- 1
	   }
	   c3 <- 1
	}()              
        
        go func() {
          for k := 0; k < 1000000; k++ {
		  c2 <-  1
	   }
	   c3 <- 1
	}()
        for{
                select {
                        case <-c1:
                                i +=1
                               continue
                        case <-c2:
                                i -=1
                               continue
                        case <-c3:
                               stop +=1
                               if stop ==2 {
                                 break
                                 }
                              continue
                }
                break //break the loop
         }         



	
    // We have no way to wait for the completion of a goroutine (without additional syncronization of some sort)
    // We'll come back to using channels in Exercise 2. For now: Sleep.
   // time.Sleep(100*time.Millisecond)
    Println("Hello from main!")
    Println("i = ",i)
}
