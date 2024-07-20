rule EICAR_Test {
    meta:
        description: "EICAR Test Rule"
    strings:
        $a = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*" ascii fullword
    condition:
        all of them
}

