// Auction_0x3280660B3bAfDAD41A774938Ab5A34Ae463EdbFe
pragma solidity ^0.4.23;
contract Auction {
  uint public price;
  bool public initialPrice = true;
  uint public timestampEnd;
  address public beneficiary;
  bool public finalized = false;

  address public owner;
  address public winner;
  mapping(address => uint) public bids;
  uint public timebound = 2; 
  uint public increaseTimeBy = 3;

  constructor(uint _price,uint _timestampEnd, address _beneficiary, uint block_timestamp) public {
    owner = msg.sender;
    price = _price;
    timestampEnd = block_timestamp+_timestampEnd;
    beneficiary = _beneficiary;
  }

  function bid(uint block_timestamp) public payable {
    require(msg.value != 0);
    require(block_timestamp < timestampEnd);
    if (bids[msg.sender] > 0) { 
      bids[msg.sender] += msg.value;
    } 
    if (bids[msg.sender] <= 0){
      bids[msg.sender] = msg.value;
    }

    if (initialPrice) {
      require(bids[msg.sender] >= price);
    } 
    if (!initialPrice){
      require(bids[msg.sender] >= price + 2);
    }
    if (block_timestamp > timestampEnd - timebound) {
      timestampEnd = block_timestamp + increaseTimeBy;
    }
    initialPrice = false;
    price = bids[msg.sender];
    winner = msg.sender;
  }

  function refund() public {
    require(msg.sender != winner);
    msg.sender.send( bids[msg.sender] );
    bids[msg.sender] = 0;
  }

}