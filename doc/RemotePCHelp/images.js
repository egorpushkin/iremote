function switchImage(id, file) {
	$(id).attr("src", file);		
}
function showImage(id, fileMac, fileWin7, fileWinXP, title) {
	var imgCode 
		= '<table>																								'
		+ '	<tr>																								'
		+ '		<td>																							'
		+ '			<img src="' + fileMac + '" id="' + id + '"/>												'
		+ '		</td>																							'
		+ '		<td valign="top">																				'
		+ '			<a href="#" onclick="switchImage(\'#' + id + '\',\'' + fileMac + '\');">Mac</a> <br/>		'
		+ '			<a href="#" onclick="switchImage(\'#' + id + '\',\'' + fileWin7 + '\');">Win 7</a> <br/>	'
		+ '			<a href="#" onclick="switchImage(\'#' + id + '\',\'' + fileWinXP + '\');">Win XP</a> <br/>	'
		+ '		</td>																							'	
		+ '	</tr>																								'
		+ '	<tr> 																								'
		+ '		<td colspan="2" align="center">	 																'									
		+ '			' + title + ' 																				'
		+ '		</td> 																							'	
		+ '	</tr> 																								'
		+ '</table> 																							';
	document.write(imgCode);
}