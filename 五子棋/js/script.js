var chess = document.getElementById('chess');
var context = chess.getContext('2d');
var visit = new Array(20);//标记位置，是否有棋子
var gameover = false;//游戏是否结束
var win = -1;//最后的赢家，0代表玩家，1代表电脑
var flag = 0;//代表当前是黑棋还是白棋
//dp[i][j][k] 表示第k种赢法需不需要i，j这个位置
var dp = new Array(20);


for(var i = 0; i < 15; i++)
{
    dp[i] = new Array(20);
    for(var j = 0; j < 15; j++)
    {
        dp[i][j] = new Array(1000);
    }
}
for(var i = 0; i < 15; i++)
{
    for(var j = 0; j < 15; j++)
    {
        for(var k = 0; k < 900; k++)
        {
            dp[i][j][k] = 0;
        }
    }
}
//总共的赢发数量
var sum = 0;

//从左到右赢
for(var i = 0; i < 15; i++)
{
    for(var j = 0; j < 15; j++)
    {
        if(i + 4 < 15)
        {
            for(var k = 0; k <= 4; k++)
            {
                dp[i + k][j][sum] = 1;
            }
            sum++;
        }
    }
}
//从上倒下赢
for(var i = 0; i < 15; i++)
{
    for(var j = 0; j < 15; j++)
    {
        if(j + 4 < 15)
        {
            for(var k = 0; k <= 4; k++)
            {
                dp[i][j + k][sum] = 1;
            }
            sum++;
        }
    }
}
//从左上到右下赢,右上到左下赢
for(var i = 0; i < 15; i++)
{
    for(var j = 0; j < 15; j++)
    {
        if(i + 4 < 15 && j + 4 < 15)
        {
            for(var k = 0; k <= 4; k++)
            {
                dp[i + k][j + k][sum] = 1;
            }
            sum++;
        }
        if(i - 4 >= 0 && j + 4 < 15)
        {
            for(var k = 0; k <= 4; k++)
            {
                dp[i - k][j + k][sum] = 1;
            }
            sum++;
        }
    }
}

var player = new Array(1000);//玩家在第i种赢发上已经放置的棋子数量，为5代表玩家赢，为-1代表玩家在这一种赢发上不可能赢
var computer = new Array(1000);//电脑在第i种赢发上已经放置的棋子数量，为5代表玩家赢，为-1代表电脑在这一种赢发不可能赢
for(var i = 0; i < 900; i++)
{
    player[i] = 0;
    computer[i] = 0;
}
for(var i = 0; i < 15; i++)
{
    visit[i] = new Array(20);
}
for(var i = 0; i < 15; i++)
{
    for(var j = 0; j < 15; j++)
    {
        visit[i][j] = -1;
    }
}
context.strokeStyle = "#BFBFBF";
//画出棋盘，每个格子大小为30 * 30
var drawChessBoard = function()
{
    for(var i = 0; i < 15; i++)
    {
       context.moveTo(15 + i * 30, 15);
       context.lineTo(15 + i * 30, 435);
       context.stroke();
       context.moveTo(15, 15 + i * 30);
       context.lineTo(435, 15 + i * 30);
       context.stroke();
    }
}
//判断谁赢
function judge(){
    if(win == 0)
    {
        //window.alert("you win");
        artDialog(
            {	
                content:'恭喜你！你赢了，是否继续？',
                fixed:true,
                left:'20%',
                top:'50%'
            },
            function(){
                window.location.reload();
            },
            function(){
                
            }
    );
    }
    else if(win == 1)
    {
       // window.alert("computer win");
       artDialog(
        {	
            content:'你输了，是否继续？',
            fixed:true,
            left:'20%',
            top:'50%'
        },
        function(){
            window.location.reload();
        },
        function(){
        }
);
    }
}
//绘制棋子
//type为0表示白棋，type为1表示黑棋
var drawChess = function(i, j, type)
{
    context.beginPath();
    context.arc(15 + i * 30, 15 + j * 30, 13, 0, 2 * Math.PI);
    context.closePath();
    var gradient = context.createRadialGradient(15 + i * 30 + 2, 15 + j * 30 - 2, 13 ,15 + i * 30 + 2, 15 + j * 30 - 2, 0);
    if(type)
    {
        gradient.addColorStop(0, "#0A0A0A");
        gradient.addColorStop(1, "#636766");
    }
    else
    {
        gradient.addColorStop(0, "#D1D1D1");
        gradient.addColorStop(1, "#F9F9F9");
    }
    var audio = new Audio("music/click.mp3");
    audio.play();
    context.fillStyle = gradient;
    context.fill();
    if(win != -1)
    {
        setTimeout("judge()",1000);
    }
}
var computerAl = function()//电脑计算下一步下哪
{
    if(gameover) return;
    var score1 = new Array(20);//电脑进行拦截玩家在位置i,j上所得的分数
    var score2 = new Array(20);//电脑自己下，让自己先赢在位置i, j上所得的分数

    var s, t;//最终决定下的位置
    var Max = 0;
    for(var i = 0; i < 15; i++)
    {
        score1[i] = new Array(20);
        score2[i] = new Array(20);
        for(var j = 0; j < 15; j++)
        {
            score1[i][j] = 0;
            score2[i][j] = 0;
        }
    }
    //遍历还没有被下子的位置
    for(var i = 0; i < 15; i++)
    {
        for(var j = 0; j < 15; j++)
        {
            if(visit[i][j] != -1) continue;
            for(var k = 0; k < sum; k++)
            {
                if(dp[i][j][k])
                {
                    //计算score1
                    if(player[k] == 1) score1[i][j] += 10;
                    else if(player[k] == 2) score1[i][j] += 30;
                    else if(player[k] == 3) score1[i][j] += 600;
                    else if(player[k] == 4) score1[i][j] += 5000;

                    //计算score2
                    if(computer[k] == 1) score2[i][j] += 10;
                    else if(computer[k] == 2) score2[i][j] += 300;
                    else if(computer[k] == 3) score2[i][j] += 500;
                    else if(computer[k] == 4) score2[i][j] += 100000;

                }
            }
            if(score1[i][j]  > Max)
            {
               Max = score1[i][j];
               s = i;
               t = j;
            }
            if(score2[i][j] > Max)
            {
                Max = score2[i][j];
                s = i;
                t = j;
            }
        }
    }
    for(var i = 0; i < sum; i++)
    {
        if(dp[s][t][i])
        {
            computer[i]++;
            if(computer[i] == 5)
            {
                gameover = true;
                //window.alert("computer win");
                win = 1;
            }
            player[i] = -1;
        }
    }
    drawChess(s, t, flag);
    visit[s][t] = flag;
    flag ^= 1;
}
drawChessBoard();
chess.onclick = function(e)
{
    if(flag == 1 || gameover) return;//当前电脑在下子
    //获得鼠标点击的位置
    var x = e.offsetX;
    var y = e.offsetY;
    var i = Math.floor(x / 30);
    var j = Math.floor(y / 30);
    if(visit[i][j] == -1)//当前位置没有棋子
    {
        for(var k = 0; k < sum; k++)//遍历所有赢发
        {
            if(dp[i][j][k] == 1 && player[k] != -1)
            {
                player[k]++;
                if(player[k] == 5)
                {
                    //window.alert("you win");
                    win = 0;
                    gameover = true;
                }
                computer[k] = -1;//电脑在这种赢发上已经不可能赢
            }
        }
        //绘制棋子
        drawChess(i, j, flag);
        //改变状态
        visit[i][j] = flag;
        flag ^= 1;
        computerAl();
    }
}