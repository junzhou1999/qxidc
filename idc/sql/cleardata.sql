delete
from T_ZHOBTMIND
where upttime < timestampadd(minute, -120, now());
