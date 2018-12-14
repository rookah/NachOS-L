#!/usr/bin/env bats

@test "prodcons" {
  skip "Lucas: Implement me when it works!"
  result="$(./nachos-step2 -rs 1 -x prodcons)"
  [ "$result" -eq "bidule" ]
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