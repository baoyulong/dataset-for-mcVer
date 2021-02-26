contract AccessControlMethod{
    address public owner;
    address public subject;
    address public object;

    struct PolicyItem{
        bool isValued;
        uint minInterval;
        uint ToLR;
        uint NoFR;
        uint threshold;
        bool result;
        uint8 err;
    }

    mapping (bytes32 => mapping(bytes32 => PolicyItem)) policies;
    mapping (bytes32 => BehaviorItem) behaviors;

    public AccessControlMethod(address _subject) public{
        owner = msg.sender;
        object = msg.sender;
        subject = _subject;
    }


    function policyAdd(uint _resource,uint _action,uint _permission, uint _minInterval, uint _threshold) public {
        uint resource = _resource;
        uint action = _action;

        if (msg.sender== owner) {
            if (policies[resource][action].isValued) {
                throw;
            } else {
                policies[resource][action].permission=_permission;
                policies[resource][action].minInterval=_minInterval;
                policies[resource][action].threshold=_threshold;
                policies[resource][action].ToLR=0;
                policies[resource][action].NoFR=0;
                policies[resource][action].isValued=true;
                policies[resource][action].result=false;
                behaviors[resource].TimeofUnblock = 0;
            }
        }
    
    }

    function policyUpdate(uint _resource, uint _action, uint _newPermission) public{
        uint resource = _resource;
        uint action = _action;
        if(policies[resource][action].isValued){
            policies[resource][action].permission = _newPermission;
        }
    }


    function minIntervalUpdate(uint _resource, uint _action, uint _newMinInterval) public{
        uint resource = _resource;
        uint action = _action;
        if(policies[resource][action].isValued){
            policies[resource][action].minInterval= _newMinInterval;
        }
    }

    function thresholdUpdate(uint _resource, uint _action, uint _newThreshold) public{
        uint resource = _resource;
        uint action = _action;
        if (policies[resource][action].isValued) {
            policies[resource][action].threshold = _newThreshold;
        } else {
            throw;
        }
    }

    function accessControl(uint _resource, uint _action, uint _time) public {
        bool policycheck = false;
        bool behaviorcheck =true;
        uint8 errcode =0;
        uint penalty = 0;

        if (msg.sender == subject) {
            uint resource = _resource;
            uint action = _action;

            if (behaviors[resource].TimeofUnblock >= _time) {
                errcode =1;
            } else {
                if (behaviors[resource].TimeofUnblock>0) {
                    behaviors[resource].TimeofUnblock = 0;
                    policies[resource][action].NoFR=0;
                    policies[resource][action]ToLR=0;
                }
                if (true == policies[resource][action].permission){
                    policycheck = true;
                }
                else{
                    policycheck = false;
                }
                //behavior check
                if (_time - policies[resource][action].ToLR <= policies[resource][action].minInterval){
                    policies[resource][action].NoFR++;
                    if(policies[resource][action].NoFR >= policies[resource][action].threshold){
                        penalty = 2;
                        behaviorcheck =false;
                        behaviors[resource].TimeofUnblock = _time + penalty* 1;
                    }
                }else{
                    policies[resource][action].NoFR=0;
                }
                if(!policycheck && behaviorcheck) errcode = 2; //"Static Check failed!"
                if(policycheck && !behaviorcheck) errcode = 3;  //"Misbehavior detected!"
                if(!policycheck && !behaviorcheck) errcode = 4; //"Static check failed! & Misbehavior detecteed
            }
            policies[resource][action].ToLR=_time;
        } else {
            errcode=5;
        }

        policies[resource][action].result = policycheck & behaviorcheck ;
        policies[resource][action].err = errcode;
    }
}
