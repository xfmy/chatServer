
recv
{
    "vsersion":"chat",
    {
        "eventType":注册
        "user":{
        "userId":20308,
        "userName":"xf666my888",
        "password1":"123123",
        "loginState":"online"
    }
    }
}

response
{
    "msg":"操作成功",
    "code":200,
    
    "main":{
        "eventType":注册
        "user":{
        "userId":20308,
        "userName":"xf666my888",
        "password1":"589bd3afd7d4fb6f050b70803ed26056",
        "loginState":"online"
        }   
    }
}




{
    "msg":"操作成功",
    "code":200,
    "version":"chat"
    "eventType":注册
    "user":{
        "userId":20308,
        "userName":"xf666my888",
        "password1":"589bd3afd7d4fb6f050b70803ed26056",
        "loginState":"online"
    }
}

网络层的那些回调函数都应该改到业务层的函数回调???????????????????????



1.network::onMessageCompleteCallback转移到distribute,通过设置回调



只给最新的版本提供服务,老版本提供更新服务

数据库jdbc  createStatement  prepareStatement
String sql = "insert into book (bookid,bookname,bookauthor,booksort,bookprice) values ('"+var1+"',
                                '"+var2+"',"+var3+",'"+var4+","+var5+"')";
stmt = conn.createStatement();
rs = stmt.executeUpdate(sql);




String sql = "insert into book (bookid,bookname,bookauthor,booksort,bookprice) values (?,?,?,?,?)";
pstmt = conn.prepareStatement(sql);
pstmt.setString(1,var1);
pstmt.setString(2,var2);
pstmt.setString(3,var3);
pstmt.setString(4,var4);
pstmt.setString(5,var5);
pstmt.executeUpdate();

做一个连接池,()->取链接
{"version":1,"user":{"name":"admin","password":"admin"}}


{"msgType":5,"id":19,"from":"pi pi","to":"zhang san","msg":"hello zhang san"}

存储用户的离线消息
insert into offlinemessage values(%d,'%s'),userid,msg

select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.user=%d ==>  id    