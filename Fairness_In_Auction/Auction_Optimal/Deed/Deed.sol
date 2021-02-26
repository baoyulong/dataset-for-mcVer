// Auction_0xed139a28ec75a336bb1dcd6e3a0aba96c8217774

pragma solidity ^0.5.0;
contract Rewrite{
  uint256 minPrice = 7;
  uint256 highestBid;
  uint256 secondhighestBid;
  address winner;

  constructor(){
      highestBid = 0;
  }

  function bid(uint msg_price) public payable{
    uint value = msg_price;
    if (msg.value<=msg_price){
        value = msg_value;
    }
    if(value>=minPrice){
      if(value>highestBid){
        secondhighestBid = highestBid; 
        highestBid = value;
        winner = msg.sender;
      }else{
        if(value>secondhighestBid){
          secondhighestBid = value;
        }
      }
    }
  }
}