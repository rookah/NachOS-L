#!/usr/bin/env bats

@test "semaphore" {
  result="$(./nachos-test -rs 1 -x semaphore | head -n 1)"
  [[ "$result" == "----------------------------------------------------------------------------------------------------" ]]
}

@test "mutex" {
  result="$(./nachos-test -rs 1 -x mutex | head -n 1)"
  [[ "$result" == "1400" ]]
}

@test "condition1" {
  result="$(./nachos-test -rs 1 -x condition1 | head -n 1)"
  [[ "$result" == "132" ]]
}

@test "condition2" {
  result="$(./nachos-test -rs 1 -x condition2 | head -n 1)"
  [[ "$result" == "123" ]]
}

@test "nohalt" {
  result="$(./nachos-test -rs 1 -x nohalt | head -n 1)"
  [[ "$result" == "This main function does not halt!" ]]
}

@test "nothreadexit" {
  result="$(./nachos-test -rs 1 -x nothreadexit | head -n 1)"
  [[ "$result" == "0-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32-33-34-35-36-37-38-39-40-41-42-43-44-45-46-47-48-49-50-51-52-53-54-55-56-57-58-59-60-61-62-63-64-65-66-67-68-69-70-71-72-73-74-75-76-77-78-79-80-81-82-83-84-85-86-87-88-89-90-91-92-93-94-95-96-97-98-99-" ]]
}

@test "prodcons" {
  result="$(./nachos-test -rs 1 -x prodcons | head -n 1)"
  [[ "$result" == "0-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32-33-34-35-36-37-38-39-40-41-42-43-44-45-46-47-48-49-50-51-52-53-54-55-56-57-58-59-60-61-62-63-64-65-66-67-68-69-70-71-72-73-74-75-76-77-78-79-80-81-82-83-84-85-86-87-88-89-90-91-92-93-94-95-96-97-98-99-" ]]
}

@test "join" {
  result="$(./nachos-test -rs 1 -x join | head -n 1)"
  [[ "$result" == "111112222233333" ]]
}

@test "getputchar" {
  result="$(echo 'a' | ./nachos-test -rs 1 -x getputchar | head -n 1)"
  [[ "$result" == "a" ]]
}


@test "getputstring" {
  result="$(echo "foobar2000" | ./nachos-test -rs 1 -x getputstring | head -n 1)"
  [[ "$result" == "foobar2000" ]]
}

@test "halt" {
  result="$(./nachos-test -rs 1 -x halt | head -n 1)"
  [[ "$result" == "Machine halting!" ]]
}

@test "multithread" {
  result="$(./nachos-test -rs 1 -x multithread | head -n 1)"
  [[ "$result" == "0003111110003333000020022222220022222222222000002220200000000000000000000000002222222220022000000000022222222200000000222000000000022220000000000000000000000000" ]]
}

@test "putchar" {
  result="$(./nachos-test -rs 1 -x putchar | head -n 1)"
  [[ "$result" == "abcd" ]]
}