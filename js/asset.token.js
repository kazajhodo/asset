(function ($, Drupal) {

$.asset = {};

/**
 * Convert tokens into assets.
 */
$.asset.toAsset = function(text) {
  var assets;
  if(Drupal.settings.asset && Drupal.settings.asset.render){
    assets = Drupal.settings.asset.render;
  }
  else if(parent && parent.Drupal.settings.asset && parent.Drupal.settings.asset.render){
    assets = parent.Drupal.settings.asset.render;
  }
  if(!assets){
    return text;
  }
  var match = function(text) {
    var match, regex;
    regex = new RegExp("((\\<p\\>)?\\[asset-[0-9]+(-[0-9]+)?\\](\\<\\/p\\>)?)", "g");
    match = void 0;
    text = text.replace(regex, function(match, text) {
      var aid, iid, base, parts;
      regex = new RegExp("(\\<p\\>)?\\[asset-([0-9]+)-?([0-9]+)?\\](\\<\\/p\\>)?", ["i"]);
      parts = match.match(regex);
      aid = parts[2];
      iid = (parts[3] ? parts[3] : 0);
      base = aid + "-" + iid;
      if (assets[base]) {
        return text.replace(regex, assets[base]);
      }
      return '';
    });
    return text;
  };
  return match(text);
};

/**
 * Covert assets to tokens.
 */
$.asset.toToken = function(html) {
  var aid, iid, $html, $asset;
  // Strip out last hanging empty <p> tag.
  var regex = new RegExp('.*(<p>(\s|&nbsp;|<\/?\s?br\s?\/?>)*<\/?p>[\n\r\f])(?![\f\n\r])');
  html = html.replace(regex, '');
  // Replace tokens
  var $html = $('<div></div>').append(html);
  $('*[data-aid]', $html).each(function(){
    $asset = $(this);
    aid = $asset.attr('data-aid');
    iid = $asset.attr('data-iid');
    $asset.replaceWith('[asset-' + aid + '-' + iid + ']');
  });
  return $html.html();
};

})(jQuery, Drupal);
