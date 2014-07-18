(function($) {

Drupal.behaviors.asset_widget_alignment = {
  attach: function (context, settings) {
    var self = this;
    $('body').once('asset-full').each(function(){
      $(window).on('resize', function(){
        $('.asset-full').each(function(){
          self.align($(this));
        });
      });
    });
    $('.asset-full', context).once('asset-full').each(function(){
      self.align($(this));
    });
  },

  align: function ($element) {
    $element.css({marginLeft:'auto',marginRight:'auto'});
    var maxWidth = $element.width();
    $('.asset-reset', $element).each(function(){
      $(this).css({maxWidth:maxWidth, marginLeft:'auto', marginRight:'auto'});
    });
    var offset = $element.offset();
    $element.css({marginLeft:(offset.left * -1) + 'px',marginRight:(offset.left * -1) + 'px'});
  }
}

})(jQuery);
