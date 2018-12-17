#!/usr/bin/env bats

@test "semaphore" {
  result="$(./nachos-step2 -rs 1 -x semaphore | head -n 1)"
  [[ "$result" == "----------------------------------------------------------------------------------------------------" ]]
}

@test "nohalt" {
  result="$(./nachos-step2 -rs 1 -x nohalt | head -n 1)"
  [[ "$result" == "This main function does not halt!" ]]
}

@test "nothreadexit" {
  result="$(./nachos-step2 -rs 1 -x nothreadexit | head -n 1)"
  [[ "$result" == "0-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32-33-34-35-36-37-38-39-40-41-42-43-44-45-46-47-48-49-50-51-52-53-54-55-56-57-58-59-60-61-62-63-64-65-66-67-68-69-70-71-72-73-74-75-76-77-78-79-80-81-82-83-84-85-86-87-88-89-90-91-92-93-94-95-96-97-98-99-" ]]
}

@test "prodcons" {
  result="$(./nachos-step2 -rs 1 -x prodcons | head -n 1)"
  [[ "$result" == "0-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32-33-34-35-36-37-38-39-40-41-42-43-44-45-46-47-48-49-50-51-52-53-54-55-56-57-58-59-60-61-62-63-64-65-66-67-68-69-70-71-72-73-74-75-76-77-78-79-80-81-82-83-84-85-86-87-88-89-90-91-92-93-94-95-96-97-98-99-" ]]
}

@test "join" {
  result="$(./nachos-step2 -rs 1 -x join | head -n 1)"
  [[ "$result" == "133333222111212" ]]
}

@test "getputchar" {
  result="$(echo 'a' | ./nachos-step2 -rs 1 -x getputchar | head -n 1)"
  [[ "$result" == "a" ]]
}


@test "getputstring" {
  result="$(echo "foobar2000" | ./nachos-step2 -rs 1 -x getputstring | head -n 1)"
  [[ "$result" == "foobar2000" ]]
}

@test "halt" {
  result="$(./nachos-step2 -rs 1 -x halt | head -n 1)"
  [[ "$result" == "Machine halting!" ]]
}

@test "multithread" {
  result="$(./nachos-step2 -rs 1 -x multithread | head -n 1)"
  [[ "$result" == "0111113330002332220000202000220222222220000000200000000000022000222220222222220000200022200022022222220000002222200000000000000000000000000000000000000000000000" ]]
}

@test "putchar" {
  result="$(./nachos-step2 -rs 1 -x putchar | head -n 1)"
  [[ "$result" == "abcd" ]]
}