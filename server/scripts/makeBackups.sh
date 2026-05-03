pw="SECRET"


date=`date +"%Y_%b_%d_%a"`

mysqldump -u jcr13_dbU --password=$pw jcr13_cdTicket > /home/jcr13/backups/cd_ticket_$date.mysql
gzip -f /home/jcr13/backups/cd_ticket_$date.mysql

mysqldump -u jcr13_dbU --password=$pw jcr13_cdGame > /home/jcr13/backups/cd_game_$date.mysql
gzip -f /home/jcr13/backups/cd_game_$date.mysql


mysqldump -u jcr13_dbU --password=$pw jcr13_cdForums > /home/jcr13/backups/cd_forums_$date.mysql
gzip -f /home/jcr13/backups/cd_forums_$date.mysql

mysqldump -u jcr13_dbU --password=$pw jcr13_cdNews > /home/jcr13/backups/cd_news_$date.mysql
gzip -f /home/jcr13/backups/cd_news_$date.mysql

# delete backup files older than two weeks                                                                              
find /home/jcr13/backups -mtime +14 -delete
