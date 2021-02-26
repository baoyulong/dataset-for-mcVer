// Auction_0x5B566B473bb0Ea8Dc0fC6047DD623E5fA3b42307
pragma solidity ^0.4.18;
contract Rewrite{
  mapping(address=>uint256) userEtherOf;
  uint256 price;
  address winnerAddr;
  uint256 endTime;

  constructor(uint256 _endTime, uint256 block_timestamp){
    winnerAddr = msg.sender;
    price = msg.value;
    endTime = block_timestamp + _endTime;
  }
  function bid(uint256 _price, uint256 block_timestamp) public payable{
    if(msg.value > 0){
      userEtherOf[msg.sender] = userEtherOf[msg.sender] + msg.value;
      if(_price >= 1){
        if(userEtherOf[msg.sender] >= _price){
          if(block_timestamp < endTime){
            if(_price > price) {
              userEtherOf[msg_sender] = userEtherOf[msg.sender] - _price;
              userEtherOf[winnerAddr] = userEtherOf[winnerAddr] + price;
              price = _price;
              winnerAddr = msg.sender;
            }
          }
        }
      }
    }
  }

}