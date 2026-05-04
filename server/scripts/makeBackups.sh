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



# now all website php stuff, which contains settings files
# that have been modified from git versions

dirName=cd_webPHP_$date
dirPath=/home/jcr13/backups/$dirName

rm -rf $dirPath
mkdir $dirPath

cd /home/jcr13/public_html
find . -type f -name '*.php' -exec cp --parents -t $dirPath {} +

cd /home/jcr13/backups

tar czf $dirName.tar.gz $dirName

rm -r $dirName



# delete backup files older than two weeks

find /home/jcr13/backups -mtime +14 -delete
